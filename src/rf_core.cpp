/**
 * @file rf_core.cpp
 * @brief RF SubGHz Core Driver (CC1101) - Custom SPI Implementation
 * 
 * Uses HSPI (SPI2) for CC1101 to avoid conflict with TFT_eSPI (FSPI/SPI3)
 * Implements raw SPI communication to CC1101 without external library
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#include "rf_core.h"
#include "pin_config.h"
#include "s3_driver.h"
#include <SPI.h>
#include <SD.h>

// ============================================================================
// CC1101 REGISTER DEFINITIONS
// ============================================================================
// Configuration registers
#define CC1101_IOCFG2   0x00
#define CC1101_IOCFG1   0x01
#define CC1101_IOCFG0   0x02
#define CC1101_FIFOTHR  0x03
#define CC1101_PKTLEN   0x06
#define CC1101_PKTCTRL1 0x07
#define CC1101_PKTCTRL0 0x08
#define CC1101_FSCTRL1  0x0B
#define CC1101_FREQ2    0x0D
#define CC1101_FREQ1    0x0E
#define CC1101_FREQ0    0x0F
#define CC1101_MDMCFG4  0x10
#define CC1101_MDMCFG3  0x11
#define CC1101_MDMCFG2  0x12
#define CC1101_MDMCFG1  0x13
#define CC1101_MDMCFG0  0x14
#define CC1101_DEVIATN  0x15
#define CC1101_MCSM2    0x16
#define CC1101_MCSM1    0x17
#define CC1101_MCSM0    0x18
#define CC1101_FOCCFG   0x19
#define CC1101_FREND1   0x21
#define CC1101_FREND0   0x22
#define CC1101_FSCAL3   0x23
#define CC1101_FSCAL2   0x24
#define CC1101_FSCAL1   0x25
#define CC1101_FSCAL0   0x26
#define CC1101_PATABLE  0x3E
#define CC1101_TXFIFO   0x3F
#define CC1101_RXFIFO   0x3F

// Strobe commands
#define CC1101_SRES     0x30  // Reset
#define CC1101_SCAL     0x33  // Calibrate
#define CC1101_SRX      0x34  // RX mode
#define CC1101_STX      0x35  // TX mode
#define CC1101_SIDLE    0x36  // Idle
#define CC1101_SFRX     0x3A  // Flush RX FIFO
#define CC1101_SFTX     0x3B  // Flush TX FIFO
#define CC1101_SNOP     0x3D  // No operation

// Status registers
#define CC1101_PARTNUM  0x30
#define CC1101_VERSION  0x31
#define CC1101_RSSI     0x34
#define CC1101_MARCSTATE 0x35

// ============================================================================
// CUSTOM HSPI FOR CC1101
// ============================================================================
static SPIClass *cc1101_spi = nullptr;
static bool cc1101_hw_present = false;

// ============================================================================
// STATIC MEMBER INITIALIZATION
// ============================================================================
bool RFCore::_initialized = false;
bool RFCore::_jammerActive = false;
bool RFCore::_receiverActive = false;
bool RFCore::_ghostActive = false;
bool RFCore::_bruteActive = false;
bool RFCore::_scannerActive = false;

float RFCore::_currentFreq = FREQ_433;
int RFCore::_currentPower = POWER_MAX;
int RFCore::_currentMod = MOD_ASK_OOK;

RFAttackType RFCore::_currentAttack = RF_ATTACK_NONE;
CapturedSignal RFCore::_lastCapture = {};
SpectrumData RFCore::_spectrumData = {};

uint16_t RFCore::_burstDuration = 100;
uint16_t RFCore::_burstPause = 50;
uint32_t RFCore::_lastBurstTime = 0;
bool RFCore::_burstState = false;

uint16_t RFCore::_ghostMinDelay = 8000;
uint16_t RFCore::_ghostMaxDelay = 45000;
uint8_t RFCore::_ghostRepeats = 5;
uint8_t RFCore::_ghostCurrentRepeat = 0;
uint32_t RFCore::_ghostNextTime = 0;

uint32_t RFCore::_bruteCurrentCode = 0;
uint32_t RFCore::_bruteTotalCodes = 0;
uint8_t RFCore::_bruteBits = 24;
RFProtocol RFCore::_bruteProtocol = PROTO_PRINCETON;

float RFCore::_scanStart = 433.0f;
float RFCore::_scanEnd = 434.0f;
float RFCore::_scanStep = 0.01f;
float RFCore::_scanCurrent = 433.0f;
uint8_t RFCore::_scanIndex = 0;

// ============================================================================
// LOW-LEVEL SPI FUNCTIONS
// ============================================================================
static void cc1101_select() {
    digitalWrite(PIN_CC1101_CS, LOW);
    delayMicroseconds(10);
}

static void cc1101_deselect() {
    digitalWrite(PIN_CC1101_CS, HIGH);
}

static uint8_t cc1101_spi_transfer(uint8_t data) {
    return cc1101_spi->transfer(data);
}

static void cc1101_write_reg(uint8_t addr, uint8_t data) {
    cc1101_select();
    cc1101_spi_transfer(addr);
    cc1101_spi_transfer(data);
    cc1101_deselect();
}

static uint8_t cc1101_read_reg(uint8_t addr) {
    cc1101_select();
    cc1101_spi_transfer(addr | 0x80);  // Read bit
    uint8_t result = cc1101_spi_transfer(0);
    cc1101_deselect();
    return result;
}

static uint8_t cc1101_read_status(uint8_t addr) {
    cc1101_select();
    cc1101_spi_transfer(addr | 0xC0);  // Status read
    uint8_t result = cc1101_spi_transfer(0);
    cc1101_deselect();
    return result;
}

static void cc1101_strobe(uint8_t cmd) {
    cc1101_select();
    cc1101_spi_transfer(cmd);
    cc1101_deselect();
}

static void cc1101_write_burst(uint8_t addr, uint8_t *data, uint8_t len) {
    cc1101_select();
    cc1101_spi_transfer(addr | 0x40);  // Burst write
    for (int i = 0; i < len; i++) {
        cc1101_spi_transfer(data[i]);
    }
    cc1101_deselect();
}

// ============================================================================
// CC1101 CONFIGURATION
// ============================================================================
static void cc1101_reset() {
    cc1101_deselect();
    delayMicroseconds(5);
    cc1101_select();
    delayMicroseconds(10);
    cc1101_deselect();
    delayMicroseconds(45);
    cc1101_strobe(CC1101_SRES);
    delay(10);
}

static void cc1101_set_frequency(float freq) {
    // CC1101 frequency formula: F = (FREQ * 26MHz) / 2^16
    uint32_t freqReg = (uint32_t)(freq * 65536.0f / 26.0f);
    cc1101_write_reg(CC1101_FREQ2, (freqReg >> 16) & 0xFF);
    cc1101_write_reg(CC1101_FREQ1, (freqReg >> 8) & 0xFF);
    cc1101_write_reg(CC1101_FREQ0, freqReg & 0xFF);
}

static void cc1101_set_mode_tx() {
    cc1101_strobe(CC1101_SIDLE);
    cc1101_strobe(CC1101_SFTX);
    cc1101_strobe(CC1101_STX);
}

static void cc1101_set_mode_rx() {
    cc1101_strobe(CC1101_SIDLE);
    cc1101_strobe(CC1101_SFRX);
    cc1101_strobe(CC1101_SRX);
}

static void cc1101_set_mode_idle() {
    cc1101_strobe(CC1101_SIDLE);
}

static void cc1101_configure_ask_ook() {
    // ASK/OOK modulation for garage doors, etc.
    cc1101_write_reg(CC1101_MDMCFG4, 0xF6);  // Data rate
    cc1101_write_reg(CC1101_MDMCFG3, 0x83);
    cc1101_write_reg(CC1101_MDMCFG2, 0x30);  // ASK/OOK, no preamble
    cc1101_write_reg(CC1101_MDMCFG1, 0x00);
    cc1101_write_reg(CC1101_FREND0, 0x11);   // PA table index
    
    // Max power
    uint8_t patable[8] = {0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    cc1101_write_burst(CC1101_PATABLE, patable, 8);
}

// ============================================================================
// INITIALIZATION
// ============================================================================
bool RFCore::init() {
    if (_initialized) return true;
    if (!CC1101_ENABLED) {
        Serial.println("[RF] CC1101 disabled in config");
        return false;
    }

    Serial.println("[RF] Initializing CC1101 on HSPI...");

    // Power ON via MOSFET
    pinMode(PIN_CC1101_EN, OUTPUT);
    digitalWrite(PIN_CC1101_EN, HIGH);
    delay(100);
    
    // Configure CS pin
    pinMode(PIN_CC1101_CS, OUTPUT);
    digitalWrite(PIN_CC1101_CS, HIGH);
    
    // Configure GDO0 pin
    pinMode(PIN_CC1101_GDO0, INPUT);
    
    // Initialize HSPI (SPI2) for CC1101 - separate from TFT FSPI
    cc1101_spi = new SPIClass(HSPI);
    cc1101_spi->begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_CC1101_CS);
    cc1101_spi->setFrequency(4000000);  // 4MHz max for CC1101
    cc1101_spi->setDataMode(SPI_MODE0);
    
    // Reset CC1101
    cc1101_reset();
    delay(10);
    
    // Check if CC1101 is present
    uint8_t partNum = cc1101_read_status(CC1101_PARTNUM);
    uint8_t version = cc1101_read_status(CC1101_VERSION);
    
    Serial.printf("[RF] CC1101 PartNum: 0x%02X, Version: 0x%02X\n", partNum, version);
    
    if (version == 0x14 || version == 0x04) {
        Serial.println("[RF] CC1101 detected!");
        cc1101_hw_present = true;
        
        // Configure for ASK/OOK @ 433MHz
        cc1101_set_frequency(433.92f);
        cc1101_configure_ask_ook();
        
        _initialized = true;
        Serial.println("[RF] CC1101 Ready!");
        return true;
    } else {
        Serial.println("[RF] CC1101 NOT DETECTED - check wiring");
        cc1101_hw_present = false;
        digitalWrite(PIN_CC1101_EN, LOW);
        return false;
    }
}

void RFCore::stop() {
    if (_initialized && cc1101_hw_present) {
        cc1101_set_mode_idle();
    }
    _initialized = false;
    _jammerActive = false;
    _receiverActive = false;
    _ghostActive = false;
    _bruteActive = false;
    _scannerActive = false;
    _currentAttack = RF_ATTACK_NONE;
}

bool RFCore::isInitialized() { return _initialized; }

// ============================================================================
// CONFIGURATION
// ============================================================================
void RFCore::setFrequency(float freq) {
    _currentFreq = freq;
    if (_initialized && cc1101_hw_present) {
        cc1101_set_frequency(freq);
    }
    Serial.printf("[RF] Frequency set to %.3f MHz\n", freq);
}

float RFCore::getFrequency() { return _currentFreq; }

void RFCore::setTxPower(int dbm) {
    _currentPower = dbm;
    // Would need different PA table entries for different power levels
}

void RFCore::setModulation(int mod) {
    _currentMod = mod;
    // Would configure CC1101 for different modulation types
}

void RFCore::setDataRate(float kbps) { }
void RFCore::setBandwidth(float khz) { }
void RFCore::setDeviation(float khz) { }
void RFCore::setSyncWord(uint16_t sync) { }

// ============================================================================
// JAMMER
// ============================================================================
void RFCore::startJammerContinuous(float freq) {
    if (!_initialized || !cc1101_hw_present) {
        Serial.println("[RF] CC1101 not available");
        return;
    }
    
    Serial.printf("[RF] Jammer Continuous @ %.3f MHz\n", freq);
    setFrequency(freq);
    cc1101_set_mode_tx();
    
    _jammerActive = true;
    _currentAttack = RF_ATTACK_JAMMER_CONTINUOUS;
}

void RFCore::startJammerBurst(float freq, uint16_t burstMs, uint16_t pauseMs) {
    if (!_initialized || !cc1101_hw_present) return;
    
    Serial.printf("[RF] Jammer Burst @ %.3f MHz (%dms on, %dms off)\n", freq, burstMs, pauseMs);
    setFrequency(freq);
    
    _burstDuration = burstMs;
    _burstPause = pauseMs;
    _lastBurstTime = millis();
    _burstState = true;
    
    cc1101_set_mode_tx();
    _jammerActive = true;
    _currentAttack = RF_ATTACK_JAMMER_BURST;
}

void RFCore::startJammerSmart(float freq) {
    if (!_initialized || !cc1101_hw_present) return;
    
    Serial.printf("[RF] Smart Jammer @ %.3f MHz\n", freq);
    setFrequency(freq);
    cc1101_set_mode_rx();  // Start in RX to detect signals
    
    _jammerActive = true;
    _currentAttack = RF_ATTACK_JAMMER_SMART;
}

void RFCore::updateJammer() {
    if (!_jammerActive || !cc1101_hw_present) return;
    
    if (_currentAttack == RF_ATTACK_JAMMER_BURST) {
        uint32_t now = millis();
        if (_burstState) {
            if (now - _lastBurstTime >= _burstDuration) {
                cc1101_set_mode_idle();
                _burstState = false;
                _lastBurstTime = now;
            }
        } else {
            if (now - _lastBurstTime >= _burstPause) {
                cc1101_set_mode_tx();
                _burstState = true;
                _lastBurstTime = now;
            }
        }
    } else if (_currentAttack == RF_ATTACK_JAMMER_SMART) {
        // Check RSSI for activity
        int8_t rssi = getRSSI();
        if (rssi > -70) {  // Signal detected
            cc1101_set_mode_tx();
            delay(10);
            cc1101_set_mode_rx();
        }
    }
}

void RFCore::stopJammer() {
    if (cc1101_hw_present) {
        cc1101_set_mode_idle();
    }
    _jammerActive = false;
    _currentAttack = RF_ATTACK_NONE;
    Serial.println("[RF] Jammer stopped");
}

bool RFCore::isJamming() { return _jammerActive; }

// ============================================================================
// CAPTURE / RECEIVE
// ============================================================================
bool RFCore::startReceive(float freq) {
    if (!_initialized || !cc1101_hw_present) return false;
    
    setFrequency(freq);
    cc1101_set_mode_rx();
    _receiverActive = true;
    Serial.printf("[RF] Receiver started @ %.3f MHz\n", freq);
    return true;
}

void RFCore::stopReceive() {
    if (cc1101_hw_present) {
        cc1101_set_mode_idle();
    }
    _receiverActive = false;
}

bool RFCore::hasSignal() {
    if (!_receiverActive || !cc1101_hw_present) return false;
    return digitalRead(PIN_CC1101_GDO0) == HIGH;
}

int8_t RFCore::getRSSI() {
    if (!cc1101_hw_present) return -128;
    
    uint8_t rssi_raw = cc1101_read_status(CC1101_RSSI);
    int8_t rssi;
    if (rssi_raw >= 128) {
        rssi = (int8_t)((rssi_raw - 256) / 2 - 74);
    } else {
        rssi = (int8_t)(rssi_raw / 2 - 74);
    }
    return rssi;
}

CapturedSignal RFCore::getLastCapture() { return _lastCapture; }
bool RFCore::isReceiving() { return _receiverActive; }

// ============================================================================
// TRANSMIT / REPLAY
// ============================================================================
bool RFCore::transmitRaw(uint8_t* data, uint16_t len) {
    if (!_initialized || !cc1101_hw_present) return false;
    
    cc1101_strobe(CC1101_SIDLE);
    cc1101_strobe(CC1101_SFTX);
    
    // Write to TX FIFO
    cc1101_select();
    cc1101_spi_transfer(CC1101_TXFIFO | 0x40);  // Burst write
    for (int i = 0; i < len && i < 64; i++) {
        cc1101_spi_transfer(data[i]);
    }
    cc1101_deselect();
    
    cc1101_strobe(CC1101_STX);
    delay(len);  // Approximate TX time
    cc1101_strobe(CC1101_SIDLE);
    
    return true;
}

bool RFCore::transmitCode(uint32_t code, uint8_t bits, RFProtocol proto) {
    if (!_initialized || !cc1101_hw_present) return false;
    
    // Encode according to protocol
    uint8_t buffer[64];
    uint16_t len = 0;
    encodeProtocol(code, bits, proto, buffer, &len);
    
    return transmitRaw(buffer, len);
}

bool RFCore::replayLast() {
    if (!_lastCapture.valid) return false;
    return transmitRaw(_lastCapture.rawData, _lastCapture.rawLength);
}

void RFCore::startGhostReplay(uint16_t minDelayMs, uint16_t maxDelayMs, uint8_t repeats) {
    _ghostMinDelay = minDelayMs;
    _ghostMaxDelay = maxDelayMs;
    _ghostRepeats = repeats;
    _ghostCurrentRepeat = 0;
    _ghostNextTime = millis() + random(minDelayMs, maxDelayMs);
    _ghostActive = true;
    _currentAttack = RF_ATTACK_REPLAY_GHOST;
    Serial.printf("[RF] Ghost Replay: %d-%dms delay, %d repeats\n", minDelayMs, maxDelayMs, repeats);
}

void RFCore::stopGhostReplay() {
    _ghostActive = false;
    _currentAttack = RF_ATTACK_NONE;
}

bool RFCore::isGhostActive() { return _ghostActive; }

// ============================================================================
// SCANNER / SPECTRUM
// ============================================================================
void RFCore::startFrequencyScan(float startFreq, float endFreq, float step) {
    _scanStart = startFreq;
    _scanEnd = endFreq;
    _scanStep = step;
    _scanCurrent = startFreq;
    _scanIndex = 0;
    _scannerActive = true;
    _currentAttack = RF_ATTACK_SCAN_FREQUENCY;
    
    _spectrumData.startFreq = startFreq;
    _spectrumData.endFreq = endFreq;
    _spectrumData.stepSize = step;
    _spectrumData.numSamples = 0;
    
    Serial.printf("[RF] Scanning %.3f - %.3f MHz\n", startFreq, endFreq);
}

void RFCore::updateScanner() {
    if (!_scannerActive || !cc1101_hw_present) return;
    
    setFrequency(_scanCurrent);
    cc1101_set_mode_rx();
    delay(2);  // Settling time
    
    int8_t rssi = getRSSI();
    if (_scanIndex < 128) {
        _spectrumData.rssiValues[_scanIndex] = rssi;
        _spectrumData.numSamples = _scanIndex + 1;
    }
    
    _scanCurrent += _scanStep;
    _scanIndex++;
    
    if (_scanCurrent > _scanEnd) {
        _scanCurrent = _scanStart;
        _scanIndex = 0;
        Serial.println("[RF] Scan cycle complete");
    }
}

void RFCore::stopFrequencyScan() {
    _scannerActive = false;
    _currentAttack = RF_ATTACK_NONE;
    if (cc1101_hw_present) cc1101_set_mode_idle();
}

SpectrumData RFCore::getSpectrumData() { return _spectrumData; }

float RFCore::findStrongestFrequency() {
    int8_t maxRssi = -128;
    uint8_t maxIdx = 0;
    for (int i = 0; i < _spectrumData.numSamples; i++) {
        if (_spectrumData.rssiValues[i] > maxRssi) {
            maxRssi = _spectrumData.rssiValues[i];
            maxIdx = i;
        }
    }
    return _spectrumData.startFreq + (maxIdx * _spectrumData.stepSize);
}

// ============================================================================
// PROTOCOL DETECTION
// ============================================================================
RFProtocol RFCore::detectProtocol(uint8_t* data, uint16_t len) { return PROTO_UNKNOWN; }
bool RFCore::decodeProtocol(CapturedSignal* signal) { return false; }

const char* RFCore::getProtocolName(RFProtocol proto) {
    switch (proto) {
        case PROTO_PRINCETON: return "Princeton";
        case PROTO_NICE_FLO: return "Nice FloR";
        case PROTO_CAME: return "CAME";
        case PROTO_LINEAR: return "Linear";
        case PROTO_HOLTEK: return "Holtek";
        case PROTO_PT2262: return "PT2262";
        case PROTO_KEELOQ: return "KeeLoq";
        default: return "Unknown";
    }
}

// ============================================================================
// BRUTE FORCE
// ============================================================================
void RFCore::startBruteForce(float freq, uint8_t bits, RFProtocol proto) {
    setFrequency(freq);
    _bruteBits = bits;
    _bruteProtocol = proto;
    _bruteCurrentCode = 0;
    _bruteTotalCodes = (1UL << bits);
    _bruteActive = true;
    _currentAttack = RF_ATTACK_BRUTE_FORCE;
    Serial.printf("[RF] Brute Force: %d bits, %lu codes\n", bits, _bruteTotalCodes);
}

void RFCore::startDeBruijn(float freq, uint8_t bits) {
    setFrequency(freq);
    _bruteBits = bits;
    _bruteActive = true;
    _currentAttack = RF_ATTACK_DEBRUIJN;
    Serial.printf("[RF] De Bruijn: %d bits\n", bits);
}

void RFCore::stopBruteForce() {
    _bruteActive = false;
    _currentAttack = RF_ATTACK_NONE;
}

bool RFCore::isBruteForcing() { return _bruteActive; }
uint32_t RFCore::getBruteProgress() { return _bruteCurrentCode; }
uint32_t RFCore::getBruteTotal() { return _bruteTotalCodes; }

void RFCore::updateBruteForce() {
    if (!_bruteActive || !cc1101_hw_present) return;
    
    transmitCode(_bruteCurrentCode, _bruteBits, _bruteProtocol);
    _bruteCurrentCode++;
    
    if (_bruteCurrentCode >= _bruteTotalCodes) {
        Serial.println("[RF] Brute force complete");
        stopBruteForce();
    }
}

// ============================================================================
// STATUS & DIAGNOSTICS
// ============================================================================
bool RFCore::selfTest() {
    return _initialized && cc1101_hw_present;
}

uint8_t RFCore::getVersion() {
    if (!cc1101_hw_present) return 0;
    return cc1101_read_status(CC1101_VERSION);
}

int8_t RFCore::getTemperature() { return 0; }

void RFCore::printStatus() {
    Serial.println("=== RF Core Status ===");
    Serial.printf("Initialized: %s\n", _initialized ? "Yes" : "No");
    Serial.printf("HW Present: %s\n", cc1101_hw_present ? "Yes" : "No");
    Serial.printf("Frequency: %.3f MHz\n", _currentFreq);
    Serial.printf("Attack: %d\n", _currentAttack);
    if (cc1101_hw_present) {
        Serial.printf("RSSI: %d dBm\n", getRSSI());
    }
}

// ============================================================================
// SAVE / LOAD
// ============================================================================
bool RFCore::saveSignal(const char* filename, CapturedSignal* sig) {
    if (!sig->valid) return false;
    
    File f = SD.open(filename, FILE_WRITE);
    if (!f) return false;
    
    f.printf("Frequency: %.6f\n", sig->frequency);
    f.printf("Protocol: %s\n", getProtocolName(sig->protocol));
    f.printf("Data: ");
    for (int i = 0; i < sig->rawLength; i++) {
        f.printf("%02X", sig->rawData[i]);
    }
    f.println();
    f.close();
    return true;
}

bool RFCore::loadSignal(const char* filename, CapturedSignal* sig) {
    // Not implemented yet
    return false;
}

bool RFCore::saveFlipperFormat(const char* filename, CapturedSignal* sig) {
    if (!sig->valid) return false;
    
    File f = SD.open(filename, FILE_WRITE);
    if (!f) return false;
    
    f.println("Filetype: Flipper SubGhz RAW File");
    f.println("Version: 1");
    f.printf("Frequency: %lu\n", (uint32_t)(sig->frequency * 1000000));
    f.println("Preset: FuriHalSubGhzPresetOok650Async");
    f.println("Protocol: RAW");
    f.close();
    return true;
}

bool RFCore::loadFlipperFormat(const char* filename, CapturedSignal* sig) {
    return false;
}

// ============================================================================
// INTERNAL HELPERS
// ============================================================================
void RFCore::sendNoise(uint16_t bytes) {
    if (!cc1101_hw_present) return;
    
    uint8_t noise[64];
    for (int i = 0; i < 64; i++) {
        noise[i] = random(256);
    }
    
    while (bytes > 0) {
        uint8_t chunk = (bytes > 64) ? 64 : bytes;
        transmitRaw(noise, chunk);
        bytes -= chunk;
    }
}

void RFCore::encodeProtocol(uint32_t code, uint8_t bits, RFProtocol proto, uint8_t* out, uint16_t* outLen) {
    // Simple encoding - just copy code bytes
    *outLen = (bits + 7) / 8;
    for (int i = 0; i < *outLen; i++) {
        out[i] = (code >> (8 * ((*outLen) - 1 - i))) & 0xFF;
    }
}
