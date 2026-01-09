#include "sound_manager.h"

PiezoDriver* SoundManager::piezo = nullptr;
bool SoundManager::_muted = false;

void SoundManager::init() {
    if (!piezo) {
        piezo = new PiezoDriver(PIN_PIEZO);
        piezo->begin();
    }
}

void SoundManager::playClick() {
    if (_muted || !piezo) return;
    // Short high blip
    piezo->tone(2000, 20);
}

void SoundManager::playBack() {
    if (_muted || !piezo) return;
    // Lower blip
    piezo->tone(1000, 30);
}

void SoundManager::playSelect() {
    if (_muted || !piezo) return;
    // Two-tone confirmation
    piezo->tone(1500, 50);
    delay(50);
    piezo->tone(2500, 80);
}

void SoundManager::playError() {
    if (_muted || !piezo) return;
    // Low buzz
    piezo->tone(200, 150);
    delay(100);
    piezo->tone(150, 300);
}

void SoundManager::playBoot() {
    if (_muted || !piezo) return;
    // Rising futuristic chime
    int notes[] = {523, 659, 784, 1046}; // C E G C
    for (int n : notes) {
        piezo->tone(n, 100);
        delay(50);
    }
}

void SoundManager::playLevelUp() {
    if (_muted || !piezo) return;
    // Victory Fanfare (Final Fantasy-ish short)
    int melody[] = {523, 523, 523, 523, 415, 466, 523, 0, 466, 523};
    int duration[] = {100, 100, 100, 300, 300, 300, 300, 50, 100, 600};
    
    for (int i=0; i<10; i++) {
        if (melody[i] == 0) delay(duration[i]);
        else piezo->tone(melody[i], duration[i]);
        delay(30);
    }
}

void SoundManager::playChirpHappy() {
    if (_muted || !piezo) return;
    // Random high chirps
    for(int i=0; i<3; i++) {
        piezo->tone(random(2000, 3000), 50);
        delay(30);
    }
}

void SoundManager::playChirpEating() {
    if (_muted || !piezo) return;
    // Gulping or munching sound
    piezo->tone(800, 50);
    delay(50);
    piezo->tone(600, 50);
}

void SoundManager::playChirpAngry() {
    if (_muted || !piezo) return;
    // Hiss sound simulation (noise)
    for(int i=0; i<5; i++) {
        piezo->tone(random(100, 300), 20);
    }
}

void SoundManager::setMute(bool mute) { _muted = mute; }
bool SoundManager::isMuted() { return _muted; }
