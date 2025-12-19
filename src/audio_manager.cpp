#include "audio_manager.h"
#include <Arduino.h>
#include <ESP8266SAM.h>
#include <AudioOutputI2S.h>

// Globals
static AudioOutputI2S *out = NULL;
static ESP8266SAM *sam = NULL;

// I2S Pins for ESP32-S3 CYD N16R8
#define I2S_BCLK 40
#define I2S_LRCK 39
#define I2S_DOUT 41

void init_tts_main() {
    out = new AudioOutputI2S();
    out->SetPinout(I2S_BCLK, I2S_LRCK, I2S_DOUT);
    sam = new ESP8266SAM();
    
    // Test beep or simple hello
    // sam->Say(out, "System Ready");
    
    log_i("Audio Manager Initialized (SAM + I2S)");
}

void speak_text_sam(const char* text) {
    if (!out || !sam) {
        init_tts_main();
    }
    
    if (out && sam) {
        log_i("Speaking: %s", text);
        sam->Say(out, text);
    }
}
