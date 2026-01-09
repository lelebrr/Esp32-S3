/**
 * @file nfc_relay.cpp
 * @brief Implementação de NFC Relay via ESP-NOW
 *
 * @author MorphNode Team
 * @date 2025-12-23
 */

#include "nfc_relay.h"
#include "module_manager.h"
#include "nfc_driver.h"
#include <WiFi.h>
#include <esp_wifi.h>


// Static member initialization
RelayMode NFCRelay::_mode = RELAY_MODE_IDLE;
uint8_t NFCRelay::_peerMac[6] = {0};
bool NFCRelay::_peerSet = false;
bool NFCRelay::_active = false;

uint8_t NFCRelay::_currentUid[10] = {0};
uint8_t NFCRelay::_currentUidLen = 0;

uint32_t NFCRelay::_packetsSent = 0;
uint32_t NFCRelay::_packetsReceived = 0;
uint32_t NFCRelay::_lastLatency = 0;
uint32_t NFCRelay::_sendTime = 0;

RelayPacket NFCRelay::_pendingRequest;
bool NFCRelay::_hasPendingRequest = false;

// ============================================================================
// INICIALIZAÇÃO
// ============================================================================

void NFCRelay::init() {
    Serial.println("[RELAY] Initializing NFC Relay...");

    // WiFi precisa estar inicializado para ESP-NOW
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Inicializa ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("[RELAY] ESP-NOW init failed!");
        return;
    }

    // Registra callbacks
    esp_now_register_send_cb(onDataSent);
    esp_now_register_recv_cb(onDataRecv);

    // Inicializa NFC
    NFCDriver::init();

    Serial.println("[RELAY] Ready");
}

void NFCRelay::setReaderMode() {
    _mode = RELAY_MODE_READER;
    _active = true;
    Serial.println("[RELAY] Mode: READER (capturing real card)");
}

void NFCRelay::setEmulatorMode() {
    _mode = RELAY_MODE_EMULATOR;
    _active = true;
    Serial.println("[RELAY] Mode: EMULATOR (presenting to terminal)");
}

void NFCRelay::setPeer(const uint8_t *mac) {
    memcpy(_peerMac, mac, 6);
    _peerSet = true;

    // Adiciona peer
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("[RELAY] Failed to add peer");
        _peerSet = false;
    } else {
        Serial.printf(
            "[RELAY] Peer: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]
        );
    }
}

bool NFCRelay::discoverPeer() {
    Serial.println("[RELAY] Discovering peer...");

    // Envia pacote de descoberta em broadcast
    RelayPacket discoverPkt;
    discoverPkt.type = RELAY_MSG_DISCOVER;

    uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    // Adiciona broadcast peer temporariamente
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcast, 6);
    peerInfo.channel = 0;
    esp_now_add_peer(&peerInfo);

    esp_now_send(broadcast, (uint8_t *)&discoverPkt, sizeof(RelayPacket));

    // Aguarda resposta (timeout 5s)
    uint32_t start = millis();
    while (!_peerSet && millis() - start < 5000) { delay(10); }

    esp_now_del_peer(broadcast);

    return _peerSet;
}

// ============================================================================
// UPDATE LOOP
// ============================================================================

void NFCRelay::update() {
    if (!_active) return;

    switch (_mode) {
        case RELAY_MODE_READER: handleReaderMode(); break;

        case RELAY_MODE_EMULATOR: handleEmulatorMode(); break;

        default: break;
    }
}

void NFCRelay::handleReaderMode() {
    // Lê cartão real
    NFCCard card = NFCDriver::readCard(100);

    if (card.valid) {
        // Cartão detectado - envia UID para emulador
        if (_currentUidLen == 0 || memcmp(_currentUid, card.uid, card.uidLength) != 0) {
            memcpy(_currentUid, card.uid, card.uidLength);
            _currentUidLen = card.uidLength;

            RelayPacket pkt;
            pkt.type = RELAY_MSG_CARD_DATA;
            memcpy(pkt.uid, card.uid, card.uidLength);
            pkt.uidLen = card.uidLength;

            sendPacket(&pkt);

            Serial.printf(
                "[RELAY] Card detected, UID sent: %s\n",
                NFCDriver::uidToString(card.uid, card.uidLength).c_str()
            );
        }
    }

    // Processa requisições pendentes do emulador
    if (_hasPendingRequest) {
        RelayPacket &req = _pendingRequest;
        RelayPacket resp;

        switch (req.type) {
            case RELAY_MSG_AUTH_REQ:
                // Autentica bloco no cartão real
                resp.type = RELAY_MSG_AUTH_RESP;
                resp.block = req.block;
                resp.success = NFCDriver::readBlock(req.block, resp.data, req.key);
                sendPacket(&resp);
                break;

            case RELAY_MSG_READ_REQ:
                // Lê bloco do cartão real
                resp.type = RELAY_MSG_READ_RESP;
                resp.block = req.block;
                resp.success = NFCDriver::readBlock(req.block, resp.data, req.key);
                sendPacket(&resp);
                break;

            case RELAY_MSG_WRITE_REQ:
                // Escreve bloco no cartão real
                resp.type = RELAY_MSG_WRITE_RESP;
                resp.block = req.block;
                resp.success = NFCDriver::writeBlock(req.block, req.data, req.key);
                sendPacket(&resp);
                break;

            default: break;
        }

        _hasPendingRequest = false;
    }
}

void NFCRelay::handleEmulatorMode() {
    // Em modo emulador, aguarda dados do reader
    // e responde às requisições do terminal

    // A emulação real requer hardware específico (como ACR122U clone mode)
    // ou uso de cartões mágicos (Gen2)

    // Nesta implementação, apenas logamos o que seria feito
    if (_currentUidLen > 0) {
        Serial.printf("[RELAY] Emulating UID: ");
        for (int i = 0; i < _currentUidLen; i++) { Serial.printf("%02X", _currentUid[i]); }
        Serial.println();
    }
}

// ============================================================================
// CALLBACKS ESP-NOW
// ============================================================================

void NFCRelay::onDataSent(const uint8_t *mac, esp_now_send_status_t status) {
    _packetsSent++;

    if (status == ESP_NOW_SEND_SUCCESS) { _lastLatency = millis() - _sendTime; }
}

void NFCRelay::onDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
    if (len != sizeof(RelayPacket)) return;

    _packetsReceived++;

    RelayPacket *pkt = (RelayPacket *)data;

    // Se recebeu discover, responde e configura peer
    if (pkt->type == RELAY_MSG_DISCOVER && !_peerSet) {
        setPeer(mac);

        // Envia resposta
        RelayPacket resp;
        resp.type = RELAY_MSG_DISCOVER;
        sendPacket(&resp);
        return;
    }

    processPacket(pkt);
}

void NFCRelay::processPacket(const RelayPacket *pkt) {
    switch (pkt->type) {
        case RELAY_MSG_CARD_DATA:
            // Recebeu dados do cartão
            memcpy(_currentUid, pkt->uid, pkt->uidLen);
            _currentUidLen = pkt->uidLen;
            Serial.printf("[RELAY] Received card UID (%d bytes)\n", pkt->uidLen);
            break;

        case RELAY_MSG_AUTH_REQ:
        case RELAY_MSG_READ_REQ:
        case RELAY_MSG_WRITE_REQ:
            // Armazena para processamento
            memcpy(&_pendingRequest, pkt, sizeof(RelayPacket));
            _hasPendingRequest = true;
            break;

        case RELAY_MSG_AUTH_RESP:
        case RELAY_MSG_READ_RESP:
        case RELAY_MSG_WRITE_RESP:
            // Resposta do reader - processa conforme contexto
            if (pkt->success) {
                Serial.printf("[RELAY] Response OK, block %d\n", pkt->block);
            } else {
                Serial.printf("[RELAY] Response FAIL, block %d\n", pkt->block);
            }
            break;

        default: break;
    }
}

bool NFCRelay::sendPacket(const RelayPacket *pkt) {
    if (!_peerSet) return false;

    _sendTime = millis();
    esp_err_t result = esp_now_send(_peerMac, (uint8_t *)pkt, sizeof(RelayPacket));

    return (result == ESP_OK);
}

// ============================================================================
// CONTROLE E STATUS
// ============================================================================

void NFCRelay::stop() {
    _active = false;
    _mode = RELAY_MODE_IDLE;
    _currentUidLen = 0;
    Serial.println("[RELAY] Stopped");
}

bool NFCRelay::isActive() { return _active; }

uint32_t NFCRelay::getPacketsSent() { return _packetsSent; }

uint32_t NFCRelay::getPacketsReceived() { return _packetsReceived; }

uint32_t NFCRelay::getLatencyMs() { return _lastLatency; }
