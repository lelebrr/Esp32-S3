/**
 * @file audio_driver.h
 * @brief Audio Driver Header - I2S + Piezo
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H

#include <Arduino.h>

class AudioDriver {
public:
    static void init();
    static void playTone(int frequency, int duration);
    static void playPiezoTone(int frequency, int duration);
    static void playBeep(int count, int frequency = 2000, int duration = 100);
    static void playSuccess();
    static void playError();
    static void playAlert();
    static void stopAudio();
    static bool isInitialized();

private:
    static bool initialized;
};

#endif // AUDIO_DRIVER_H
