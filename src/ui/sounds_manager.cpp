#include "sounds_manager.h"
#include "../hardware/audio_driver.h"
#include <FS.h>
#include <SD_MMC.h>
#include <debug_log.h>

SoundsManager sounds_manager;

// ═══════════════════════════════════════════════════════════════════════════
// CONSTRUCTOR
// ═══════════════════════════════════════════════════════════════════════════
SoundsManager::SoundsManager() : _isPlaying(false), _currentSoundId(0) {

    // Default configuration
    _config.masterVolume = 80;
    _config.soundOnHandshake = true;
    _config.soundOnBoot = true;
    _config.soundOnAttack = true;
    _config.handshakeSoundId = PRESET_KA_CHING;
    _config.bootSoundId = PRESET_POWERUP;
    _config.attackSoundId = PRESET_ATTACK;
}

// ═══════════════════════════════════════════════════════════════════════════
// INITIALIZATION
// ═══════════════════════════════════════════════════════════════════════════
void SoundsManager::begin() {
    LOG_SOUND("Initializing sounds manager...");

    // Ensure sounds directory exists
    if (!SD_MMC.exists(SOUNDS_DIR)) {
        LOG_SOUND("Creating sounds directory...");
        SD_MMC.mkdir(SOUNDS_DIR);
    }

    // Scan for sounds
    int count = scanSounds();
    LOG_SOUND("Found %d sounds (%d presets + %d custom)", count, PRESET_COUNT, count - PRESET_COUNT);
}

// ═══════════════════════════════════════════════════════════════════════════
// SOUND LIBRARY SCANNING
// ═══════════════════════════════════════════════════════════════════════════
int SoundsManager::scanSounds() {
    _library.clear();

    // Add preset sounds (built-in)
    const char *presetNames[] = {"beep_short", "beep_double", "beep_long", "ka_ching",     "confirm",
                                 "error",      "warning",     "success",   "notification", "alert",
                                 "powerup",    "powerdown",   "click",     "swipe",        "unlock",
                                 "lock",       "capture",     "attack",    "scan",         "complete"};

    for (int i = 0; i < PRESET_COUNT; i++) {
        SoundInfo info;
        snprintf(info.filename, sizeof(info.filename), "preset_%s.wav", presetNames[i]);
        info.filesize = 0;
        info.duration_ms = 200;
        info.isPreset = true;
        _library.push_back(info);
    }

    // Scan custom sounds from SD
    if (SD_MMC.exists(SOUNDS_DIR)) {
        File dir = SD_MMC.open(SOUNDS_DIR);
        if (dir && dir.isDirectory()) {
            File file = dir.openNextFile();
            while (file) {
                String name = file.name();
                if ((name.endsWith(".wav") || name.endsWith(".WAV")) && !name.startsWith("preset_")) {

                    SoundInfo info;
                    strncpy(info.filename, name.c_str(), sizeof(info.filename) - 1);
                    info.filename[sizeof(info.filename) - 1] = '\0';
                    info.filesize = file.size();
                    // Estimate duration: 16kHz 16-bit mono = 32000 bytes/sec
                    info.duration_ms = (file.size() * 1000) / 32000;
                    info.isPreset = false;

                    _library.push_back(info);
                    LOG_SOUND("Found: %s (%u bytes, ~%dms)", info.filename, info.filesize, info.duration_ms);
                }
                file = dir.openNextFile();
            }
            dir.close();
        }
    }

    return _library.size();
}

// ═══════════════════════════════════════════════════════════════════════════
// PLAYBACK
// ═══════════════════════════════════════════════════════════════════════════
bool SoundsManager::play(uint8_t soundId) {
    if (soundId >= _library.size()) { return false; }

    return playByName(_library[soundId].filename);
}

bool SoundsManager::playByName(const char *filename) {
    if (!filename || strlen(filename) == 0) { return false; }

    char fullPath[128];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", SOUNDS_DIR, filename);

    LOG_SOUND("Playing: %s (volume: %d%%)", fullPath, _config.masterVolume);

    // Set volume before playing
    audioDriver.setVolume(_config.masterVolume);

    // Open WAV file from SD card
    File file = SD_MMC.open(fullPath, FILE_READ);
    if (!file) {
        LOG_SOUND("Failed to open: %s", fullPath);
        return false;
    }

    // Parse WAV header (44 bytes standard)
    if (file.size() < 44) {
        LOG_SOUND("File too small for WAV");
        file.close();
        return false;
    }

    uint8_t header[44];
    file.read(header, 44);

    // Verify RIFF header
    if (header[0] != 'R' || header[1] != 'I' || header[2] != 'F' || header[3] != 'F') {
        LOG_SOUND("Not a valid WAV file");
        file.close();
        return false;
    }

    // Extract WAV parameters
    uint16_t audioFormat = header[20] | (header[21] << 8);
    uint16_t numChannels = header[22] | (header[23] << 8);
    uint32_t sampleRate = header[24] | (header[25] << 8) | (header[26] << 16) | (header[27] << 24);
    uint16_t bitsPerSample = header[34] | (header[35] << 8);
    uint32_t dataSize = header[40] | (header[41] << 8) | (header[42] << 16) | (header[43] << 24);

    LOG_SOUND("WAV: %dHz, %d-bit, %d ch, %u bytes", sampleRate, bitsPerSample, numChannels, dataSize);

    // For now, just play a success sound (actual streaming requires more setup)
    // Full WAV streaming would need I2S DMA buffer handling
    audioDriver.playSound(SOUND_SUCCESS);

    file.close();
    _isPlaying = true;
    return true;
}

bool SoundsManager::playPreset(uint8_t presetId) {
    if (presetId >= PRESET_COUNT) { return false; }

    LOG_SOUND("Playing preset: %d", presetId);

    // Map preset IDs to audio driver sounds
    switch (presetId) {
        case PRESET_BEEP_SHORT: audioDriver.playTone(880, 100); break;
        case PRESET_BEEP_DOUBLE:
            audioDriver.playTone(880, 80);
            delay(100);
            audioDriver.playTone(880, 80);
            break;
        case PRESET_BEEP_LONG: audioDriver.playTone(660, 300); break;
        case PRESET_KA_CHING:
            audioDriver.playTone(1047, 50);
            delay(50);
            audioDriver.playTone(1319, 50);
            delay(50);
            audioDriver.playTone(1568, 150);
            break;
        case PRESET_CONFIRM: audioDriver.playSound(SOUND_SUCCESS); break;
        case PRESET_ERROR: audioDriver.playSound(SOUND_ERROR); break;
        case PRESET_WARNING: audioDriver.playSound(SOUND_ALERT); break;
        case PRESET_SUCCESS: audioDriver.playSound(SOUND_SUCCESS); break;
        case PRESET_NOTIFICATION:
            audioDriver.playTone(523, 100);
            delay(80);
            audioDriver.playTone(659, 150);
            break;
        case PRESET_ALERT: audioDriver.playSound(SOUND_ALERT); break;
        case PRESET_POWERUP: audioDriver.playSound(SOUND_BOOT); break;
        case PRESET_POWERDOWN:
            audioDriver.playTone(440, 100);
            delay(50);
            audioDriver.playTone(349, 100);
            delay(50);
            audioDriver.playTone(262, 200);
            break;
        case PRESET_CLICK: audioDriver.playSound(SOUND_CLICK); break;
        case PRESET_SWIPE:
            audioDriver.playTone(400, 30);
            delay(20);
            audioDriver.playTone(500, 30);
            break;
        case PRESET_UNLOCK:
            audioDriver.playTone(523, 80);
            delay(50);
            audioDriver.playTone(659, 80);
            delay(50);
            audioDriver.playTone(784, 120);
            break;
        case PRESET_LOCK:
            audioDriver.playTone(784, 80);
            delay(50);
            audioDriver.playTone(659, 80);
            delay(50);
            audioDriver.playTone(523, 120);
            break;
        case PRESET_CAPTURE: audioDriver.playSound(SOUND_HANDSHAKE); break;
        case PRESET_ATTACK: audioDriver.playSound(SOUND_ATTACK_START); break;
        case PRESET_SCAN: audioDriver.playSound(SOUND_SCAN_START); break;
        case PRESET_COMPLETE: audioDriver.playSound(SOUND_ATTACK_COMPLETE); break;
        default: audioDriver.playSound(SOUND_BEEP); break;
    }

    return true;
}

void SoundsManager::stop() {
    audioDriver.stop();
    _isPlaying = false;
}

// ═══════════════════════════════════════════════════════════════════════════
// VOLUME
// ═══════════════════════════════════════════════════════════════════════════
void SoundsManager::setVolume(uint8_t percent) {
    _config.masterVolume = constrain(percent, 0, 100);
    LOG_SOUND("Volume: %d%%", _config.masterVolume);

    // Apply volume to audio driver
    audioDriver.setVolume(_config.masterVolume);
}

// ═══════════════════════════════════════════════════════════════════════════
// EVENT TRIGGERS
// ═══════════════════════════════════════════════════════════════════════════
void SoundsManager::playHandshakeSound() {
    if (_config.soundOnHandshake) { play(_config.handshakeSoundId); }
}

void SoundsManager::playBootSound() {
    if (_config.soundOnBoot) { play(_config.bootSoundId); }
}

void SoundsManager::playAttackSound() {
    if (_config.soundOnAttack) { play(_config.attackSoundId); }
}

// ═══════════════════════════════════════════════════════════════════════════
// CONFIGURATION
// ═══════════════════════════════════════════════════════════════════════════
void SoundsManager::setConfig(const SoundsConfig &config) {
    _config = config;
    setVolume(config.masterVolume);
}

// ═══════════════════════════════════════════════════════════════════════════
// FILE OPERATIONS
// ═══════════════════════════════════════════════════════════════════════════
bool SoundsManager::deleteSound(const char *filename) {
    // Don't allow deleting presets
    if (strncmp(filename, "preset_", 7) == 0) {
        LOG_SOUND("Cannot delete preset sounds");
        return false;
    }

    char fullPath[128];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", SOUNDS_DIR, filename);

    if (SD_MMC.remove(fullPath)) {
        LOG_SOUND("Deleted: %s", filename);
        scanSounds(); // Refresh library
        return true;
    }
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════
// TEST
// ═══════════════════════════════════════════════════════════════════════════
void SoundsManager::test(uint8_t soundId) {
    LOG_SOUND("Testing sound #%d", soundId);
    play(soundId);
}
