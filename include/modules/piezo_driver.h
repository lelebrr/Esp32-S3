#pragma once

#include <Arduino.h>

class PiezoDriver {
public:
    PiezoDriver(int pin);
    void begin();
    
    // Basic Control
    void tone(unsigned int frequency, unsigned long duration = 0);
    void noTone();
    
    // Effects
    void beep(int times = 1, int duration = 100);
    void check(); // System check beep
    
    // Attacks / Annoyance
    void siren(bool active);
    void mosquito(bool active);
    void noiseAttack(bool active);
    
    // Melodies (Non-blocking tick should be called in loop if we implement non-blocking)
    // For now, these might be blocking or short
    void playBootSound();
    void playErrorSound();

private:
    int _pin;
    int _channel; // LEDC channel
    bool _sirenActive;
    bool _mosquitoActive;
    bool _noiseActive;
    unsigned long _lastUpdate;
    int _state;
};
