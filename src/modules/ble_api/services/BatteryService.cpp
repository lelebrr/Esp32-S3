#include "BatteryService.hpp"
#include <NimBLEDevice.h>

BatteryService::BatteryService() : LeleBLEService() {}

BatteryService::~BatteryService() {
    if (battery_task_handle != nullptr) {
        vTaskDelete(battery_task_handle);
    }
}

void BatteryService::setup(NimBLEServer *pServer) {
    // Create Battery Service (0x180F)
    pService = pServer->createService("180F");

    // Battery Level Characteristic (0x2A19)
    battery_char = pService->createCharacteristic(
        "2A19",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    pService->start();
    pServer->getAdvertising()->addServiceUUID(pService->getUUID());
}

void BatteryService::end() {
    if (battery_task_handle != nullptr) {
        vTaskDelete(battery_task_handle);
        battery_task_handle = nullptr;
    }
}
