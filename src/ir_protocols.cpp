/**
 * @file ir_protocols.cpp
 * @brief Implementação de Protocolos IR Multi-formato
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#include "ir_protocols.h"

// Static member initialization
IRsend IRProtocols::_irsend(IR_SEND_PIN);
IRrecv IRProtocols::_irrecv(IR_RECV_PIN);
decode_results IRProtocols::_results;
bool IRProtocols::_initialized = false;

bool IRProtocols::_bruteActive = false;
IRProtocolType IRProtocols::_bruteProtocol = IR_PROTO_NEC;
uint64_t IRProtocols::_bruteStart = 0;
uint64_t IRProtocols::_bruteEnd = 0;
uint64_t IRProtocols::_bruteCurrent = 0;
uint16_t IRProtocols::_bruteDelay = 50;
uint32_t IRProtocols::_bruteLastTime = 0;

// ============================================================================
// TV POWER CODES DATABASE
// ============================================================================

// Samsung (NEC-like)
static const uint64_t TV_SAMSUNG_POWER = 0xE0E040BF;
static const uint16_t TV_SAMSUNG_BITS = 32;

// LG (NEC)
static const uint64_t TV_LG_POWER = 0x20DF10EF;
static const uint16_t TV_LG_BITS = 32;

// Sony (SIRC)
static const uint64_t TV_SONY_POWER = 0xA90;
static const uint16_t TV_SONY_BITS = 12;

// Panasonic
static const uint64_t TV_PANASONIC_POWER = 0x400401003D;
static const uint16_t TV_PANASONIC_BITS = 48;

// Philips (RC5)
static const uint64_t TV_PHILIPS_POWER = 0x0C;
static const uint16_t TV_PHILIPS_BITS = 12;

// ============================================================================
// INICIALIZAÇÃO
// ============================================================================

void IRProtocols::init() {
    if (_initialized) return;
    
    Serial.println("[IR] Initializing multi-protocol IR...");
    
    _irsend.begin();
    _irrecv.enableIRIn();
    
    _initialized = true;
    Serial.println("[IR] Ready: NEC, RC5, Sony, Samsung, Panasonic, RAW");
}

// ============================================================================
// ENVIO DE CÓDIGOS
// ============================================================================

void IRProtocols::send(IRProtocolType protocol, uint64_t code, uint16_t bits) {
    if (!_initialized) init();
    
    switch (protocol) {
        case IR_PROTO_NEC:
            _irsend.sendNEC(code, bits ? bits : 32);
            break;
            
        case IR_PROTO_RC5:
            _irsend.sendRC5(code, bits ? bits : 12);
            break;
            
        case IR_PROTO_RC6:
            _irsend.sendRC6(code, bits ? bits : 20);
            break;
            
        case IR_PROTO_SONY:
            _irsend.sendSony(code, bits ? bits : 12, 2);  // 2 repeats
            break;
            
        case IR_PROTO_SAMSUNG:
            _irsend.sendSAMSUNG(code, bits ? bits : 32);
            break;
            
        case IR_PROTO_LG:
            _irsend.sendLG(code, bits ? bits : 28);
            break;
            
        case IR_PROTO_PANASONIC:
            _irsend.sendPanasonic64(code, bits ? bits : 48);
            break;
            
        default:
            Serial.println("[IR] Unknown protocol!");
            break;
    }
    
    Serial.printf("[IR] Sent: proto=%d code=0x%llX bits=%d\n", protocol, code, bits);
}

void IRProtocols::sendRaw(const uint16_t* data, uint16_t length, uint16_t frequency) {
    if (!_initialized) init();
    _irsend.sendRaw(data, length, frequency);
}

// ============================================================================
// CAPTURA
// ============================================================================

bool IRProtocols::available() {
    return _irrecv.decode(&_results);
}

IRCode IRProtocols::capture() {
    IRCode result = {IR_PROTO_RAW, 0, 0, 0, nullptr, false};
    
    if (_irrecv.decode(&_results)) {
        result.valid = true;
        result.code = _results.value;
        result.bits = _results.bits;
        
        // Map decode_type to our enum
        switch (_results.decode_type) {
            case NEC: result.protocol = IR_PROTO_NEC; break;
            case RC5: result.protocol = IR_PROTO_RC5; break;
            case RC6: result.protocol = IR_PROTO_RC6; break;
            case SONY: result.protocol = IR_PROTO_SONY; break;
            case SAMSUNG: result.protocol = IR_PROTO_SAMSUNG; break;
            case LG: result.protocol = IR_PROTO_LG; break;
            case PANASONIC: result.protocol = IR_PROTO_PANASONIC; break;
            default: result.protocol = IR_PROTO_RAW; break;
        }
        
        Serial.printf("[IR] Captured: %s 0x%llX (%d bits)\n",
                     getProtocolName(result.protocol), result.code, result.bits);
        
        _irrecv.resume();
    }
    
    return result;
}

const char* IRProtocols::getProtocolName(IRProtocolType proto) {
    switch (proto) {
        case IR_PROTO_NEC: return "NEC";
        case IR_PROTO_RC5: return "RC5";
        case IR_PROTO_RC6: return "RC6";
        case IR_PROTO_SONY: return "Sony";
        case IR_PROTO_SAMSUNG: return "Samsung";
        case IR_PROTO_LG: return "LG";
        case IR_PROTO_PANASONIC: return "Panasonic";
        case IR_PROTO_RAW: return "RAW";
        default: return "Unknown";
    }
}

// ============================================================================
// BRUTE FORCE
// ============================================================================

void IRProtocols::startBrute(IRProtocolType protocol, uint64_t startCode, uint64_t endCode, uint16_t delayMs) {
    _bruteActive = true;
    _bruteProtocol = protocol;
    _bruteStart = startCode;
    _bruteEnd = endCode;
    _bruteCurrent = startCode;
    _bruteDelay = delayMs;
    _bruteLastTime = 0;
    
    Serial.printf("[IR] Brute started: %s 0x%llX -> 0x%llX\n",
                 getProtocolName(protocol), startCode, endCode);
}

bool IRProtocols::updateBrute() {
    if (!_bruteActive) return false;
    
    uint32_t now = millis();
    if (now - _bruteLastTime < _bruteDelay) return true;
    
    _bruteLastTime = now;
    
    // Determine bits based on protocol
    uint16_t bits = 32;
    switch (_bruteProtocol) {
        case IR_PROTO_NEC: bits = 32; break;
        case IR_PROTO_RC5: bits = 12; break;
        case IR_PROTO_SONY: bits = 12; break;
        default: bits = 32; break;
    }
    
    send(_bruteProtocol, _bruteCurrent, bits);
    
    _bruteCurrent++;
    
    if (_bruteCurrent > _bruteEnd) {
        _bruteActive = false;
        Serial.println("[IR] Brute completed");
        return false;
    }
    
    return true;
}

void IRProtocols::stopBrute() {
    _bruteActive = false;
    Serial.println("[IR] Brute stopped");
}

uint8_t IRProtocols::getBruteProgress() {
    if (!_bruteActive || _bruteEnd <= _bruteStart) return 100;
    
    uint64_t total = _bruteEnd - _bruteStart;
    uint64_t done = _bruteCurrent - _bruteStart;
    
    return (uint8_t)((done * 100) / total);
}

// ============================================================================
// TV POWER
// ============================================================================

void IRProtocols::sendTVPower(uint8_t brand) {
    if (!_initialized) init();
    
    switch (brand) {
        case 0: // Samsung
            _irsend.sendSAMSUNG(TV_SAMSUNG_POWER, TV_SAMSUNG_BITS);
            Serial.println("[IR] Samsung Power");
            break;
            
        case 1: // LG
            _irsend.sendNEC(TV_LG_POWER, TV_LG_BITS);
            Serial.println("[IR] LG Power");
            break;
            
        case 2: // Sony
            _irsend.sendSony(TV_SONY_POWER, TV_SONY_BITS, 2);
            Serial.println("[IR] Sony Power");
            break;
            
        case 3: // Panasonic
            _irsend.sendPanasonic64(TV_PANASONIC_POWER, TV_PANASONIC_BITS);
            Serial.println("[IR] Panasonic Power");
            break;
            
        case 4: // Philips
            _irsend.sendRC5(TV_PHILIPS_POWER, TV_PHILIPS_BITS);
            Serial.println("[IR] Philips Power");
            break;
            
        default:
            Serial.println("[IR] Unknown brand");
            break;
    }
}

void IRProtocols::tvBGone() {
    Serial.println("[IR] TV-B-Gone: Sending all power codes...");
    
    for (uint8_t brand = 0; brand <= 4; brand++) {
        sendTVPower(brand);
        delay(100);
    }
    
    // Additional common codes
    // Toshiba
    _irsend.sendNEC(0x02FD48B7, 32);
    delay(50);
    
    // Sharp
    _irsend.sendSharp(0x5EA1, 15);
    delay(50);
    
    // Sanyo
    _irsend.sendNEC(0x1CE348B7, 32);
    delay(50);
    
    Serial.println("[IR] TV-B-Gone complete");
}

// ============================================================================
// PORTÃO / GARAGE
// ============================================================================

void IRProtocols::sendGate(uint32_t code, uint8_t bits) {
    // Portões geralmente usam timings mais longos
    // Simula protocolo Princeton/PT2262 via IR
    
    uint16_t rawData[128];
    int idx = 0;
    
    // Preamble
    rawData[idx++] = 350;   // Sync pulse
    rawData[idx++] = 10850; // Sync gap
    
    // Data bits
    for (int8_t b = bits - 1; b >= 0; b--) {
        if (code & (1 << b)) {
            // Bit 1: long-short
            rawData[idx++] = 1050;
            rawData[idx++] = 350;
        } else {
            // Bit 0: short-long
            rawData[idx++] = 350;
            rawData[idx++] = 1050;
        }
    }
    
    _irsend.sendRaw(rawData, idx, 38);
    
    Serial.printf("[IR] Gate code: 0x%X (%d bits)\n", code, bits);
}

void IRProtocols::bruteGate12bit() {
    Serial.println("[IR] Gate brute 12-bit starting...");
    
    for (uint32_t code = 0; code < 4096; code++) {
        sendGate(code, 12);
        delay(30);
        
        if (code % 256 == 0) {
            Serial.printf("[IR] Gate brute: %lu/4096 (%.1f%%)\n", 
                         code, (float)code / 4096 * 100);
        }
    }
    
    Serial.println("[IR] Gate brute complete");
}
