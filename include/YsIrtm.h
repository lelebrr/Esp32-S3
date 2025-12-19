#ifndef YS_IRTM_H
#define YS_IRTM_H

#include <Arduino.h>

// YS-IRTM Operation Codes
#define YS_IRTM_CMD_TX      0xF1  // Send IR Code
#define YS_IRTM_CMD_ADDR    0xF2  // Change Device Address
#define YS_IRTM_CMD_BAUD    0xF3  // Change Baud Rate

// Default module address
#define YS_IRTM_DEFAULT_ADDR 0xA1

class YsIrtm {
public:
    YsIrtm(int8_t rxPin, int8_t txPin, HardwareSerial* serial = &Serial1);

    // Initialization
    void begin(uint32_t baud = 9600);
    void end();

    // Transmission
    // NEC Protocol: Address (2 bytes user code) + Command (1 byte - module generates inverse)
    void sendNEC(uint16_t userCode, uint8_t commandCode);
    
    // Configuration
    void setAddress(uint8_t newAddr);
    void setBaudRate(uint32_t baud);

    // Reception
    // Returns true if a full NEC code (3 bytes) was received
    bool read(uint16_t* userCode, uint8_t* commandCode);

private:
    int8_t _rxPin;
    int8_t _txPin;
    HardwareSerial* _serial;
    uint8_t _deviceAddr;

    void sendPacket(uint8_t op, uint8_t data1, uint8_t data2, uint8_t data3);
};

#endif // YS_IRTM_H
