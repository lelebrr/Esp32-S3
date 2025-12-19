#include "YsIrtm.h"

YsIrtm::YsIrtm(int8_t rxPin, int8_t txPin, HardwareSerial* serial) {
    _rxPin = rxPin;
    _txPin = txPin;
    _serial = serial;
    _deviceAddr = YS_IRTM_DEFAULT_ADDR;
}

void YsIrtm::begin(uint32_t baud) {
    // Note: ESP32 RX pin connects to Module TXD
    //       ESP32 TX pin connects to Module RXD
    _serial->begin(baud, SERIAL_8N1, _rxPin, _txPin);
}

void YsIrtm::end() {
    _serial->end();
}

void YsIrtm::sendPacket(uint8_t op, uint8_t data1, uint8_t data2, uint8_t data3) {
    uint8_t packet[5];
    packet[0] = _deviceAddr;
    packet[1] = op;
    packet[2] = data1;
    packet[3] = data2;
    packet[4] = data3;
    
    _serial->write(packet, 5);
}

void YsIrtm::sendNEC(uint16_t userCode, uint8_t commandCode) {
    // Split 16-bit user code into high and low bytes
    uint8_t userHigh = (userCode >> 8) & 0xFF;
    uint8_t userLow = userCode & 0xFF;
    
    // Op: F1 (Transmit)
    // Data1: User Code High
    // Data2: User Code Low
    // Data3: Command Code
    sendPacket(YS_IRTM_CMD_TX, userHigh, userLow, commandCode);
}

void YsIrtm::setAddress(uint8_t newAddr) {
    if (newAddr == 0x00) return; // Invalid?
    sendPacket(YS_IRTM_CMD_ADDR, newAddr, 0x00, 0x00);
    _deviceAddr = newAddr;
}

void YsIrtm::setBaudRate(uint32_t baud) {
    uint8_t baudIdx = 0;
    switch(baud) {
        case 4800: baudIdx = 1; break;
        case 9600: baudIdx = 2; break;
        case 19200: baudIdx = 3; break;
        case 57600: baudIdx = 4; break;
        default: return; // Unsupported
    }
    
    sendPacket(YS_IRTM_CMD_BAUD, baudIdx, 0x00, 0x00);
    delay(100); // Wait for module to switch
    _serial->updateBaudRate(baud);
}

bool YsIrtm::read(uint16_t* userCode, uint8_t* commandCode) {
    // Module sends 3 bytes upon receiving a valid signal:
    // [User Code High] [User Code Low] [Command Code]
    
    if (_serial->available() >= 3) {
        uint8_t buf[3];
        _serial->readBytes(buf, 3);
        
        uint8_t userHigh = buf[0];
        uint8_t userLow = buf[1];
        
        *userCode = (userHigh << 8) | userLow;
        *commandCode = buf[2];
        
        return true;
    }
    
    return false;
}
