/**
 * @file esp_now_mesh.h
 * ESP-NOW Mesh Networking Header
 * Monster S3 Firmware
 */

#ifndef ESP_NOW_MESH_H
#define ESP_NOW_MESH_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// Forward declarations
class MeshSystem {
public:
    static void init();
    static void sendChat(const uint8_t *mac, const char *message, bool broadcast = false);
    static void onChat(void (*callback)(const uint8_t *mac, const char *sender, const char *text));

private:
    static void (*chatCallback)(const uint8_t *mac, const char *sender, const char *text);
};

#endif // ESP_NOW_MESH_H
