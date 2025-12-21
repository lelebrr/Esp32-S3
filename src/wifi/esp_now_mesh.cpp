/**
 * @file esp_now_mesh.cpp
 * ESP-NOW Mesh Networking Implementation
 * Monster S3 Firmware
 */

#include "wifi/esp_now_mesh.h"

// Static member initialization
void (*MeshSystem::chatCallback)(const uint8_t *mac, const char *sender, const char *text) = nullptr;

void MeshSystem::init() {
    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("[MESH] ESP-NOW init failed");
        return;
    }

    // Register callback for received data
    esp_now_register_recv_cb([](const uint8_t *mac, const uint8_t *data, int len) {
        if (chatCallback && len > 0) {
            // Simple chat message handling
            char sender[20];
            sprintf(sender, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

            char message[256];
            memcpy(message, data, min(len, (int)sizeof(message) - 1));
            message[min(len, (int)sizeof(message) - 1)] = '\0';

            chatCallback(mac, sender, message);
        }
    });

    Serial.println("[MESH] ESP-NOW initialized");
}

void MeshSystem::sendChat(const uint8_t *mac, const char *message, bool broadcast) {
    if (!message) return;

    uint8_t targetMac[6];
    if (broadcast) {
        memset(targetMac, 0xFF, 6); // Broadcast address
    } else if (mac) {
        memcpy(targetMac, mac, 6);
    } else {
        return;
    }

    esp_now_send(targetMac, (uint8_t *)message, strlen(message));
}

void MeshSystem::onChat(void (*callback)(const uint8_t *mac, const char *sender, const char *text)) {
    chatCallback = callback;
}
