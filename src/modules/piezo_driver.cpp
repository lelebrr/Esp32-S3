#include "modules/piezo_driver.h"

// ESP32-S3 LEDC Channel for Tone
#define PIEZO_LEDC_CHANNEL 0 

PiezoDriver::PiezoDriver(int pin) {
    _pin = pin;
    _sirenActive = false;
    _mosquitoActive = false;
    _noiseActive = false;
    _lastUpdate = 0;
    _state = 0;
}

void PiezoDriver::begin() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    // Setup logic if using raw LEDC, but Arduino's tone() handles it usually.
    // For ESP32 specific tone control, we might rely on the standard library or ledcSetup.
    // On ESP32 core 3.x, tone() uses LEDC automatically.
}

void PiezoDriver::tone(unsigned int frequency, unsigned long duration) {
    if (frequency == 0) {
        noTone();
        return;
    }
    // ESP32 specific tone implementation calls
    ::tone(_pin, frequency, duration);
}

void PiezoDriver::noTone() {
    ::noTone(_pin);
    digitalWrite(_pin, LOW);
}

void PiezoDriver::beep(int times, int duration) {
    for (int i = 0; i < times; i++) {
        tone(2000, duration); // 2kHz beep
        delay(duration);
        noTone();
        if (i < times - 1) delay(100);
    }
}

void PiezoDriver::check() {
    tone(4000, 100);
    delay(100);
    tone(4500, 100);
    delay(100);
    noTone();
}

void PiezoDriver::playBootSound() {
    tone(1000, 100); delay(100);
    tone(1500, 100); delay(100);
    tone(2000, 200); delay(200);
    noTone();
}

void PiezoDriver::playErrorSound() {
    tone(500, 200); delay(200);
    tone(300, 400); delay(400);
    noTone();
}

// --- ATTACKS / FX (Non-blocking usually required, but implemented blocking for simple trigger or loop) ---

void PiezoDriver::siren(bool active) {
    _sirenActive = active;
    if (!active) {
        noTone();
        return;
    }
    
    // Blocking Siren (Simple) - To make non-blocking requires loop() integration
    // For now, we do a short burst loop then return, or expect loop calling
    // Let's make it a blocking 3-cycle siren for "demo"
    for(int k=0; k<3; k++){
        for (int i = 1000; i < 3000; i += 50) {
            tone(i);
            delay(5);
        }
        for (int i = 3000; i > 1000; i -= 50) {
            tone(i);
            delay(5);
        }
    }
    noTone();
    _sirenActive = false; // Reset
}

void PiezoDriver::mosquito(bool active) {
    _mosquitoActive = active;
    if (active) {
        tone(17000); // 17kHz - hard to hear for older adults, annoying for youth
    } else {
        noTone();
    }
}

void PiezoDriver::noiseAttack(bool active) {
    _noiseActive = active;
    if (!active) {
        noTone();
        return;
    }
    // Random annoying beeps
    unsigned long now = millis();
    if (now - _lastUpdate > 100) {
        int r = random(500, 5000);
        tone(r);
        delay(random(10, 100)); // Blocking briefly
        _lastUpdate = now;
    }
}
