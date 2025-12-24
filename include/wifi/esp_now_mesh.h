/**
 * @file esp_now_mesh.h
 * @brief ESP-NOW Mesh + NFC Relay System
 */

#ifndef ESP_NOW_MESH_H
#define ESP_NOW_MESH_H

#include <Arduino.h>
#include <esp_now.h>

class MeshSystem {
public:
    // Inicialização
    static void init();
    static bool isInitialized();
    
    // Gerenciamento de peers
    static void addPeer(const uint8_t* mac);
    
    // Chat
    static void sendChat(const uint8_t* mac, const char* message, bool broadcast = false);
    static void onChat(void (*callback)(const uint8_t *mac, const char *sender, const char *text));
    
    // NFC Relay
    static void sendNFCRelay(const uint8_t* uid, uint8_t uidLen, const uint8_t* sectorData, uint16_t dataLen);
    static void onNFCRelay(void (*callback)(const uint8_t *uid, uint8_t uidLen, const uint8_t *data, uint16_t dataLen));
    
    // Modo Relay
    static void startRelayMode(bool asReader);  // true=leitor, false=emissor
    static void stopRelayMode();
    static bool isRelayActive();
    
    // Comandos remotos
    static void sendCommand(const uint8_t* mac, const char* command);
    
    // Utilitários
    static String getMacAddress();
};

#endif // ESP_NOW_MESH_H
