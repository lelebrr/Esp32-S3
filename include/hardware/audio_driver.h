/**
 * @file audio_driver.h
 * Audio Driver Header
 * Monster S3 Firmware
 */

#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H

#include <Arduino.h>

// Audio driver class for I2S audio output and piezo buzzer
class AudioDriver {
public:
    static void init();
    static void playTone(int frequency, int duration);
    static void playPiezoTone(int frequency, int duration);
    static void stopAudio();

private:
    static bool initialized;
};

#endif // AUDIO_DRIVER_H
