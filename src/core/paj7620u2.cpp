/**
 * @file paj7620u2.cpp
 * @brief PAJ7620U2 Gesture Sensor Implementation
 */

#include "paj7620u2.h"

#ifdef HAS_GESTURE

#include <globals.h>

// ============================================================================
// STATIC VARIABLES
// ============================================================================

static TwoWire* i2c = nullptr;
static paj7620_state_t state = {
    .initialized = false,
    .chipId = 0,
    .lastGesture = GESTURE_NONE,
    .gestureCount = 0
};

// PAJ7620U2 initialization sequence
static const uint8_t initRegister[][2] = {
    {0xEF, 0x00}, // Bank 0
    {0x37, 0x07},
    {0x38, 0x17},
    {0x39, 0x06},
    {0x42, 0x01},
    {0x46, 0x2D},
    {0x47, 0x0F},
    {0x48, 0x3C},
    {0x49, 0x00},
    {0x4A, 0x1E},
    {0x4C, 0x20},
    {0x51, 0x10},
    {0x5E, 0x10},
    {0x60, 0x27},
    {0x80, 0x42},
    {0x81, 0x44},
    {0x82, 0x04},
    {0x8B, 0x01},
    {0x90, 0x06},
    {0x95, 0x0A},
    {0x96, 0x0C},
    {0x97, 0x05},
    {0x9A, 0x14},
    {0x9C, 0x3F},
    {0xA5, 0x19},
    {0xCC, 0x19},
    {0xCD, 0x0B},
    {0xCE, 0x13},
    {0xCF, 0x64},
    {0xD0, 0x21},
    {0xEF, 0x01}, // Bank 1
    {0x02, 0x0F},
    {0x03, 0x10},
    {0x04, 0x02},
    {0x25, 0x01},
    {0x27, 0x39},
    {0x28, 0x7F},
    {0x29, 0x08},
    {0x3E, 0xFF},
    {0x5E, 0x3D},
    {0x65, 0x96},
    {0x67, 0x97},
    {0x69, 0xCD},
    {0x6A, 0x01},
    {0x6D, 0x2C},
    {0x6E, 0x01},
    {0x72, 0x01},
    {0x73, 0x35},
    {0x74, 0x00},
    {0x77, 0x01},
    {0xEF, 0x00}, // Back to Bank 0
};

// ============================================================================
// I2C HELPERS
// ============================================================================

static bool writeReg(uint8_t reg, uint8_t value) {
    i2c->beginTransmission(PAJ7620_ADDR);
    i2c->write(reg);
    i2c->write(value);
    return i2c->endTransmission() == 0;
}

static uint8_t readReg(uint8_t reg) {
    i2c->beginTransmission(PAJ7620_ADDR);
    i2c->write(reg);
    i2c->endTransmission(false);
    
    i2c->requestFrom((uint8_t)PAJ7620_ADDR, (uint8_t)1);
    if (i2c->available()) {
        return i2c->read();
    }
    return 0;
}

static void selectBank(uint8_t bank) {
    writeReg(PAJ7620_BANK_SEL, bank);
}

// ============================================================================
// INITIALIZATION
// ============================================================================

bool paj7620_init(TwoWire* wire) {
    i2c = wire ? wire : &Wire;
    
    // Wake sensor from sleep
    i2c->beginTransmission(PAJ7620_ADDR);
    i2c->endTransmission();
    delay(5);
    
    i2c->beginTransmission(PAJ7620_ADDR);
    i2c->endTransmission();
    delay(5);
    
    // Check chip ID
    selectBank(0);
    state.chipId = readReg(PAJ7620_CHIP_ID);
    
    if (state.chipId != 0x20) {
        #if CORE_DEBUG_LEVEL > 0
        Serial.printf("[PAJ7620] Invalid chip ID: 0x%02X (expected 0x20)\n", state.chipId);
        #endif
        return false;
    }
    
    // Write initialization sequence
    for (size_t i = 0; i < sizeof(initRegister) / 2; i++) {
        if (!writeReg(initRegister[i][0], initRegister[i][1])) {
            #if CORE_DEBUG_LEVEL > 0
            Serial.printf("[PAJ7620] Init failed at reg 0x%02X\n", initRegister[i][0]);
            #endif
            return false;
        }
    }
    
    state.initialized = true;
    
    #if CORE_DEBUG_LEVEL > 0
    Serial.println("[PAJ7620] Gesture sensor initialized");
    #endif
    
    return true;
}

// ============================================================================
// GESTURE DETECTION
// ============================================================================

paj7620_gesture_t paj7620_get_gesture() {
    if (!state.initialized) return GESTURE_NONE;
    
    selectBank(0);
    uint8_t flag1 = readReg(PAJ7620_INT_FLAG1);
    uint8_t flag2 = readReg(PAJ7620_INT_FLAG2);
    
    paj7620_gesture_t gesture = GESTURE_NONE;
    
    if (flag1 & 0x01) gesture = GESTURE_RIGHT;
    else if (flag1 & 0x02) gesture = GESTURE_LEFT;
    else if (flag1 & 0x04) gesture = GESTURE_UP;
    else if (flag1 & 0x08) gesture = GESTURE_DOWN;
    else if (flag1 & 0x10) gesture = GESTURE_FORWARD;
    else if (flag1 & 0x20) gesture = GESTURE_BACKWARD;
    else if (flag1 & 0x40) gesture = GESTURE_CLOCKWISE;
    else if (flag1 & 0x80) gesture = GESTURE_COUNTERCLOCKWISE;
    else if (flag2 & 0x01) gesture = GESTURE_WAVE;
    
    if (gesture != GESTURE_NONE) {
        state.lastGesture = gesture;
        state.gestureCount++;
    }
    
    return gesture;
}

paj7620_gesture_t paj7620_get_last_gesture() {
    return state.lastGesture;
}

const char* paj7620_gesture_name(paj7620_gesture_t gesture) {
    switch (gesture) {
        case GESTURE_RIGHT:             return "Right";
        case GESTURE_LEFT:              return "Left";
        case GESTURE_UP:                return "Up";
        case GESTURE_DOWN:              return "Down";
        case GESTURE_FORWARD:           return "Forward";
        case GESTURE_BACKWARD:          return "Backward";
        case GESTURE_CLOCKWISE:         return "CW";
        case GESTURE_COUNTERCLOCKWISE:  return "CCW";
        case GESTURE_WAVE:              return "Wave";
        default:                        return "None";
    }
}

// ============================================================================
// INTERRUPT CONTROL
// ============================================================================

void paj7620_enable_interrupt() {
    if (!state.initialized) return;
    
    selectBank(1);
    writeReg(0x72, 0x01);  // Enable INT
    selectBank(0);
}

void paj7620_disable_interrupt() {
    if (!state.initialized) return;
    
    selectBank(1);
    writeReg(0x72, 0x00);  // Disable INT
    selectBank(0);
}

// ============================================================================
// POWER MANAGEMENT
// ============================================================================

void paj7620_sleep() {
    if (!state.initialized) return;
    
    selectBank(0);
    writeReg(0x03, 0x01);  // Enter sleep
}

void paj7620_wake() {
    if (!state.initialized) return;
    
    // Send dummy I2C transaction to wake
    i2c->beginTransmission(PAJ7620_ADDR);
    i2c->endTransmission();
    delay(5);
    
    selectBank(0);
    writeReg(0x03, 0x00);  // Exit sleep
}

// ============================================================================
// STATE & NAVIGATION
// ============================================================================

paj7620_state_t* paj7620_get_state() {
    return &state;
}

uint8_t paj7620_to_nav_action(paj7620_gesture_t gesture) {
    switch (gesture) {
        case GESTURE_UP:
        case GESTURE_FORWARD:
            return 1;  // Up / Previous
        case GESTURE_DOWN:
        case GESTURE_BACKWARD:
            return 2;  // Down / Next
        case GESTURE_LEFT:
            return 3;  // Back / Escape
        case GESTURE_RIGHT:
        case GESTURE_WAVE:
            return 4;  // Select / Enter
        default:
            return 0;  // No action
    }
}

#endif // HAS_GESTURE
