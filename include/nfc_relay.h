/**
 * @file nfc_relay.h
 * @brief NFC Relay via ESP-NOW
 * 
 * Permite relay de cartões NFC entre dois dispositivos ESP32
 * usando ESP-NOW para comunicação em tempo real.
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#ifndef NFC_RELAY_H
#define NFC_RELAY_H

#include <Arduino.h>
#include <esp_now.h>

// Tipos de mensagem relay
typedef enum {
    RELAY_MSG_DISCOVER,      // Descoberta de peer
    RELAY_MSG_CARD_DATA,     // Dados do cartão
    RELAY_MSG_AUTH_REQ,      // Requisição de autenticação
    RELAY_MSG_AUTH_RESP,     // Resposta de autenticação
    RELAY_MSG_READ_REQ,      // Requisição de leitura
    RELAY_MSG_READ_RESP,     // Resposta de leitura
    RELAY_MSG_WRITE_REQ,     // Requisição de escrita
    RELAY_MSG_WRITE_RESP     // Resposta de escrita
} RelayMessageType;

// Pacote relay
typedef struct __attribute__((packed)) {
    RelayMessageType type;
    uint8_t uid[10];
    uint8_t uidLen;
    uint8_t block;
    uint8_t data[16];
    uint8_t key[6];
    bool success;
} RelayPacket;

// Modo de operação
typedef enum {
    RELAY_MODE_IDLE,
    RELAY_MODE_READER,   // Dispositivo que lê o cartão original
    RELAY_MODE_EMULATOR  // Dispositivo que emula para o terminal
} RelayMode;

class NFCRelay {
public:
    /**
     * @brief Inicializa sistema de relay
     */
    static void init();
    
    /**
     * @brief Configura como leitor (captura cartão real)
     */
    static void setReaderMode();
    
    /**
     * @brief Configura como emulador (apresenta ao terminal)
     */
    static void setEmulatorMode();
    
    /**
     * @brief Define MAC do peer
     */
    static void setPeer(const uint8_t* mac);
    
    /**
     * @brief Descobre peer automaticamente
     */
    static bool discoverPeer();
    
    /**
     * @brief Processa relay (chamar no loop)
     */
    static void update();
    
    /**
     * @brief Para relay
     */
    static void stop();
    
    /**
     * @brief Verifica se relay está ativo
     */
    static bool isActive();
    
    /**
     * @brief Retorna estatísticas
     */
    static uint32_t getPacketsSent();
    static uint32_t getPacketsReceived();
    static uint32_t getLatencyMs();

private:
    static RelayMode _mode;
    static uint8_t _peerMac[6];
    static bool _peerSet;
    static bool _active;
    
    static uint8_t _currentUid[10];
    static uint8_t _currentUidLen;
    
    static uint32_t _packetsSent;
    static uint32_t _packetsReceived;
    static uint32_t _lastLatency;
    static uint32_t _sendTime;
    
    static RelayPacket _pendingRequest;
    static bool _hasPendingRequest;
    
    // Callbacks ESP-NOW
    static void onDataSent(const uint8_t* mac, esp_now_send_status_t status);
    static void onDataRecv(const uint8_t* mac, const uint8_t* data, int len);
    
    // Handlers
    static void handleReaderMode();
    static void handleEmulatorMode();
    static void processPacket(const RelayPacket* pkt);
    
    // Helpers
    static bool sendPacket(const RelayPacket* pkt);
};

#endif // NFC_RELAY_H
