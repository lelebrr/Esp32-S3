#pragma once
#include "LeleBLEService.hpp"
#include <NimBLEServer.h>

class BatteryService : public LeleBLEService {
    NimBLECharacteristic *battery_char = nullptr;
    TaskHandle_t battery_task_handle = nullptr;
    friend void runLowbattBLE();

public:
    BatteryService(/* args */);
    ~BatteryService() override;
    void setup(NimBLEServer *pServer) override;
    void end() override;
};
