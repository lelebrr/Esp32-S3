#ifndef LELE_BLE_SERVICE_H
#define LELE_BLE_SERVICE_H

#include <NimBLEServer.h>

class LeleBLEService {
protected:
    NimBLEService *pService = nullptr;
    uint16_t mtu = 23; // default MTU size
public:
    virtual ~LeleBLEService() = default;
    virtual void setup(NimBLEServer *pServer) = 0;
    virtual void end() = 0;
    void setMTU(int new_mtu) { mtu = new_mtu; }
};

#endif // LELE_BLE_SERVICE_H
