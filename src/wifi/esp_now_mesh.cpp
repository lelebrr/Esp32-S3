/**
 * @file esp_now_mesh.cpp
 * @brief ESP-NOW Mesh + NFC Relay System
 * 
 * Funcionalidades:
 * - Relay de NFC entre dois ESP32-S3 (20-100m)
 * - Chat broadcast entre dispositivos
 * - Sincronização de tempo via DS3231
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#include "wifi/esp_now_mesh.h"
#include "nfc_driver.h"
#include "tts_espeak.h"
#include <esp_now.h>
#include <WiFi.h>

// ============================================================================
// CALLBACK POINTERS
// ============================================================================
static void (*chatCallback)(const uint8_t *mac, const char *sender, const char *text) = nullptr;
static void (*nfcRelayCallback)(const uint8_t *uid, uint8_t uidLen, const uint8_t *data, uint16_t dataLen) = nullptr;

// ============================================================================
// MESSAGE TYPES
// ============================================================================
#define MSG_TYPE_CHAT     0x01
#define MSG_TYPE_NFC      0x02
#define MSG_TYPE_COMMAND  0x03
#define MSG_TYPE_ACK      0x04

// ============================================================================
// NFC RELAY PACKET
// ============================================================================
typedef struct __attribute__((packed)) {
    uint8_t type;           // MSG_TYPE_NFC
    uint8_t uidLen;         // UID length (4, 7, or 10)
    uint8_t uid[10];        // Card UID
    uint16_t dataLen;       // Sector data length
    uint8_t sectorData[64]; // Cloned sector data
    uint32_t timestamp;     // Sync timestamp
} NFCRelayPacket;

// ============================================================================
// CHAT PACKET
// ============================================================================
typedef struct __attribute__((packed)) {
    uint8_t type;           // MSG_TYPE_CHAT
    char sender[16];        // Sender name
    char message[200];      // Message content
} ChatPacket;

// ============================================================================
// ESTADO
// ============================================================================
static bool mesh_initialized = false;
static uint8_t relay_target_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  // Broadcast default
static bool relay_mode = false;

// ============================================================================
// CALLBACK DE RECEPÇÃO
// ============================================================================
static void onDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
    if (len < 1) return;
    
    uint8_t msgType = data[0];
    
    switch (msgType) {
        case MSG_TYPE_CHAT: {
            if (chatCallback && len >= sizeof(ChatPacket)) {
                ChatPacket* pkt = (ChatPacket*)data;
                char sender[20];
                sprintf(sender, "%02X:%02X:%02X:%02X:%02X:%02X", 
                        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                chatCallback(mac, pkt->sender, pkt->message);
            }
            break;
        }
        
        case MSG_TYPE_NFC: {
            if (nfcRelayCallback && len >= sizeof(NFCRelayPacket)) {
                NFCRelayPacket* pkt = (NFCRelayPacket*)data;
                
                Serial.printf("[RELAY] NFC recebido: UID len=%d, data len=%d\n", 
                              pkt->uidLen, pkt->dataLen);
                
                // Callback para processar cartão
                nfcRelayCallback(pkt->uid, pkt->uidLen, pkt->sectorData, pkt->dataLen);
                
                // TTS
                extern void tts_speak(const char*);
                tts_speak("cartao_recebido");
            }
            break;
        }
        
        case MSG_TYPE_COMMAND: {
            // Comandos remotos
            Serial.printf("[MESH] Comando recebido: %s\n", (char*)&data[1]);
            break;
        }
        
        case MSG_TYPE_ACK: {
            Serial.println("[MESH] ACK recebido");
            break;
        }
    }
}

// ============================================================================
// CALLBACK DE ENVIO
// ============================================================================
static void onDataSent(const uint8_t *mac, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        Serial.println("[MESH] Enviado com sucesso");
    } else {
        Serial.println("[MESH] Falha no envio");
    }
}

// ============================================================================
// INICIALIZAÇÃO
// ============================================================================
void MeshSystem::init() {
    if (mesh_initialized) return;
    
    // WiFi precisa estar em modo STA ou APSTA
    if (WiFi.getMode() == WIFI_OFF) {
        WiFi.mode(WIFI_STA);
    }
    
    // Inicializa ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("[MESH] Falha ao inicializar ESP-NOW");
        return;
    }
    
    // Registra callbacks
    esp_now_register_recv_cb(onDataRecv);
    esp_now_register_send_cb(onDataSent);
    
    // Adiciona peer broadcast
    esp_now_peer_info_t broadcastPeer = {};
    memset(broadcastPeer.peer_addr, 0xFF, 6);
    broadcastPeer.channel = 0;
    broadcastPeer.encrypt = false;
    
    if (esp_now_add_peer(&broadcastPeer) != ESP_OK) {
        Serial.println("[MESH] Falha ao adicionar peer broadcast");
    }
    
    mesh_initialized = true;
    Serial.println("[MESH] ESP-NOW inicializado");
    Serial.printf("[MESH] MAC: %s\n", WiFi.macAddress().c_str());
}

// ============================================================================
// ADICIONAR PEER (RELAY TARGET)
// ============================================================================
void MeshSystem::addPeer(const uint8_t* mac) {
    if (!mesh_initialized) return;
    
    memcpy(relay_target_mac, mac, 6);
    
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) == ESP_OK) {
        Serial.printf("[MESH] Peer adicionado: %02X:%02X:%02X:%02X:%02X:%02X\n",
                      mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
}

// ============================================================================
// CHAT
// ============================================================================
void MeshSystem::sendChat(const uint8_t* mac, const char* message, bool broadcast) {
    if (!mesh_initialized || !message) return;
    
    ChatPacket pkt;
    pkt.type = MSG_TYPE_CHAT;
    strncpy(pkt.sender, "Monster_S3", sizeof(pkt.sender));
    strncpy(pkt.message, message, sizeof(pkt.message));
    
    uint8_t targetMac[6];
    if (broadcast) {
        memset(targetMac, 0xFF, 6);
    } else if (mac) {
        memcpy(targetMac, mac, 6);
    } else {
        return;
    }
    
    esp_now_send(targetMac, (uint8_t*)&pkt, sizeof(ChatPacket));
}

void MeshSystem::onChat(void (*callback)(const uint8_t *mac, const char *sender, const char *text)) {
    chatCallback = callback;
}

// ============================================================================
// NFC RELAY
// ============================================================================
void MeshSystem::sendNFCRelay(const uint8_t* uid, uint8_t uidLen, const uint8_t* sectorData, uint16_t dataLen) {
    if (!mesh_initialized) return;
    
    NFCRelayPacket pkt;
    pkt.type = MSG_TYPE_NFC;
    pkt.uidLen = uidLen;
    memcpy(pkt.uid, uid, min(uidLen, (uint8_t)10));
    pkt.dataLen = min(dataLen, (uint16_t)64);
    memcpy(pkt.sectorData, sectorData, pkt.dataLen);
    pkt.timestamp = millis();
    
    Serial.printf("[RELAY] Enviando NFC: UID len=%d\n", uidLen);
    
    esp_now_send(relay_target_mac, (uint8_t*)&pkt, sizeof(NFCRelayPacket));
    
    // TTS
    extern void tts_speak(const char*);
    tts_speak("relay_ativo");
}

void MeshSystem::onNFCRelay(void (*callback)(const uint8_t *uid, uint8_t uidLen, const uint8_t *data, uint16_t dataLen)) {
    nfcRelayCallback = callback;
}

// ============================================================================
// MODO RELAY
// ============================================================================
void MeshSystem::startRelayMode(bool asReader) {
    if (!mesh_initialized) return;
    
    relay_mode = true;
    
    if (asReader) {
        Serial.println("[RELAY] Modo LEITOR: aguardando cartão...");
        
        // Callback para enviar cartão lido
        // TODO: Integrar com NFCDriver
    } else {
        Serial.println("[RELAY] Modo EMISSOR: aguardando dados...");
        
        // Callback para emular cartão recebido
        onNFCRelay([](const uint8_t* uid, uint8_t uidLen, const uint8_t* data, uint16_t dataLen) {
            Serial.println("[RELAY] Cartão recebido, emulando...");
            // TODO: Emular cartão com PN532
        });
    }
}

void MeshSystem::stopRelayMode() {
    relay_mode = false;
    Serial.println("[RELAY] Modo relay desativado");
}

bool MeshSystem::isRelayActive() {
    return relay_mode;
}

// ============================================================================
// COMANDOS REMOTOS
// ============================================================================
void MeshSystem::sendCommand(const uint8_t* mac, const char* command) {
    if (!mesh_initialized || !command) return;
    
    uint8_t pkt[64];
    pkt[0] = MSG_TYPE_COMMAND;
    strncpy((char*)&pkt[1], command, 62);
    pkt[63] = 0;
    
    uint8_t targetMac[6];
    if (mac) {
        memcpy(targetMac, mac, 6);
    } else {
        memset(targetMac, 0xFF, 6);  // Broadcast
    }
    
    esp_now_send(targetMac, pkt, strlen(command) + 2);
}

// ============================================================================
// STATUS
// ============================================================================
bool MeshSystem::isInitialized() {
    return mesh_initialized;
}

String MeshSystem::getMacAddress() {
    return WiFi.macAddress();
}
