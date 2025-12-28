#include "YsIrtm.h"
#include "pin_config.h"

YsIrtm::YsIrtm(int8_t rxPin, int8_t txPin, HardwareSerial *serial) {
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

void YsIrtm::end() { _serial->end(); }

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
    switch (baud) {
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

bool YsIrtm::read(uint16_t *userCode, uint8_t *commandCode) {
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

// TV-B-Gone style IR nuke function - Complete implementation with 50+ TV codes
void ir_tv_nuke() {
    Serial.println("[IR] TV Nuke activated - Sending power codes for all brands");

    YsIrtm ir(PIN_YS_IR_RX, PIN_YS_IR_TX, &Serial1);
    ir.begin(9600);
    delay(100);

    // =========================================================================
    // COMPREHENSIVE TV POWER CODE DATABASE
    // Format: {userCode, commandCode} for NEC protocol
    // =========================================================================
    
    struct TVCode {
        uint16_t userCode;
        uint8_t command;
        const char* brand;
    };
    
    static const TVCode tv_codes[] = {
        // === SAMSUNG ===
        {0x0707, 0x02, "Samsung"},
        {0xE0E0, 0x40, "Samsung"},
        {0x0707, 0x98, "Samsung Alt"},
        {0x2020, 0x40, "Samsung Old"},
        
        // === LG ===
        {0x0408, 0x08, "LG"},
        {0x20DF, 0x10, "LG"},
        {0x0408, 0x40, "LG Alt"},
        {0x20DF, 0x08, "LG Old"},
        
        // === SONY ===
        {0x4CB3, 0x15, "Sony"},
        {0x4CB3, 0x95, "Sony Alt"},
        {0x1DE2, 0x40, "Sony Bravia"},
        {0x000A, 0x15, "Sony Old"},
        
        // === PHILIPS ===
        {0x0586, 0x0C, "Philips"},
        {0x0200, 0x0C, "Philips Alt"},
        {0x0586, 0x40, "Philips Old"},
        
        // === PANASONIC ===
        {0x4004, 0x00, "Panasonic"},
        {0x807F, 0x3D, "Panasonic"},
        {0x4004, 0x3D, "Panasonic Alt"},
        
        // === TOSHIBA ===
        {0x02FD, 0x12, "Toshiba"},
        {0x40BF, 0x12, "Toshiba"},
        {0x02FD, 0x48, "Toshiba Alt"},
        
        // === MARCAS BRASILEIRAS ===
        {0x807F, 0x12, "Philco"},
        {0x00FF, 0x12, "CCE"},
        {0x00FF, 0x40, "Semp"},
        {0x40BF, 0x40, "Semp Toshiba"},
        {0x00FF, 0x45, "AOC"},
        {0x40BF, 0x45, "AOC Alt"},
        {0x609F, 0x08, "Gradiente"},
        {0x00FF, 0x08, "Britania"},
        {0x807F, 0x48, "Mondial"},
        {0x40BF, 0x48, "Lenoxx"},
        {0x00FF, 0x02, "H-Buster"},
        
        // === TCL / HISENSE ===
        {0x0586, 0x08, "TCL"},
        {0x00FF, 0x50, "TCL Alt"},
        {0x6104, 0x08, "Hisense"},
        
        // === OUTRAS MARCAS ===
        {0x00FF, 0x00, "Generic 1"},
        {0xFF00, 0x00, "Generic 2"},
        {0x0000, 0x12, "Generic 3"},
        {0xFFFF, 0x12, "Generic 4"},
        {0x1CE3, 0x08, "Sharp"},
        {0x00FF, 0x0A, "Sanyo"},
        {0x40BF, 0x0A, "Daewoo"},
        {0x20DF, 0x0A, "JVC"},
        {0x4CB3, 0x0A, "Aiwa"},
        
        // === ROKU / STREAMING ===
        {0xEA15, 0x57, "Roku"},
        {0x0000, 0x57, "Fire TV"},
        
        // === PROJETORES ===
        {0x0586, 0x00, "Epson"},
        {0x1898, 0x00, "BenQ"},
        {0x00FF, 0x60, "Optoma"},
        
        // === MONITORES ===
        {0x20DF, 0x00, "Dell"},
        {0x0707, 0x00, "Asus"},
        {0x4004, 0x00, "Acer"},
    };
    
    const int total_codes = sizeof(tv_codes) / sizeof(tv_codes[0]);
    
    Serial.printf("[IR] Sending %d power codes...\n", total_codes);
    
    // Send each code multiple times with variations
    for (int pass = 0; pass < 3; pass++) {
        for (int i = 0; i < total_codes; i++) {
            // Send NEC code
            ir.sendNEC(tv_codes[i].userCode, tv_codes[i].command);
            delay(40);  // Inter-code delay
            
            // Also try inverted command (some TVs use this)
            ir.sendNEC(tv_codes[i].userCode, ~tv_codes[i].command & 0xFF);
            delay(40);
            
            // Progress indication every 10 codes
            if ((i + 1) % 10 == 0) {
                Serial.printf("[IR] Progress: %d/%d codes sent (pass %d/3)\n", 
                              i + 1, total_codes, pass + 1);
            }
        }
        delay(500);  // Pause between passes
    }
    
    Serial.printf("[IR] TV Nuke completed - %d codes x 3 passes = %d transmissions\n", 
                  total_codes, total_codes * 3 * 2);
}

