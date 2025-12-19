#pragma once

#include "LeleBLEService.hpp"
#include <NimBLEDevice.h>

class UberService : public LeleBLEService {
    NimBLECharacteristic *request_char = nullptr;
public:
    UberService();
    ~UberService() override;
    void setup(NimBLEServer *pServer) override;
    void end() override;
    String getRideRequest();
};
