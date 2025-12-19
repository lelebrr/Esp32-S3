#include "audio.h"
#include "core/mykeyboard.h"

#if defined(HAS_NS4168_SPKR)
#include "AudioFileSourceFunction.h"
#include "AudioGeneratorAAC.h"
#include "AudioGeneratorFLAC.h"
#include "AudioGeneratorMIDI.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SNoDAC.h"
#include <ESP8266Audio.h>
#include <ESP8266SAM.h>
bool playAudioFile(FS *fs, String filepath) {
    if (!leleConfig.soundEnabled) return false;

    AudioFileSource *source = new AudioFileSourceFS(*fs, filepath.c_str());
    if (!source) return false;

    AudioOutputI2S *audioout =
        new AudioOutputI2S(); // https://github.com/earlephilhower/ESP8266Audio/blob/master/src/AudioOutputI2S.cpp#L32
    audioout->SetPinout(BCLK, WCLK, DOUT, MCLK);

    // set volume, derived from
    // https://github.com/earlephilhower/ESP8266Audio/blob/master/examples/WebRadio/WebRadio.ino
    audioout->SetGain(((float)leleConfig.soundVolume) / 100.0);

    AudioGenerator *generator = NULL;

    // switch on extension
    filepath.toLowerCase(); // case-insensitive match
    if (filepath.endsWith(".txt") || filepath.endsWith(".rtttl")) generator = new AudioGeneratorRTTTL();
    if (filepath.endsWith(".wav")) generator = new AudioGeneratorWAV();
    if (filepath.endsWith(".mod")) generator = new AudioGeneratorMOD();
    if (filepath.endsWith(".opus")) generator = new AudioGeneratorOpus();
    if (filepath.endsWith(".aac")) generator = new AudioGeneratorAAC();
    if (filepath.endsWith(".flac")) generator = new AudioGeneratorFLAC();
    // OGG Vorbis is not supported https://github.com/earlephilhower/ESP8266Audio/issues/84
    if (filepath.endsWith(".mp3")) {
        generator = new AudioGeneratorMP3();
        source = new AudioFileSourceID3(source);
    }
    /* MIDI support with configurable soundfont */
    if(filepath.endsWith(".mid"))  {
      // Soundfont configurável - tenta carregar de diferentes locais
      AudioFileSource* sf2 = NULL;
      String soundfontPath = leleConfig.customPath.length() > 0 
          ? leleConfig.customPath + "/soundfont.sf2" 
          : "/soundfont.sf2";
      
      if(setupSdCard()) {
          sf2 = new AudioFileSourceFS(SD, soundfontPath.c_str());
          if(!sf2 || !((AudioFileSourceFS*)sf2)->isOpen()) {
              delete sf2;
              sf2 = new AudioFileSourceFS(SD, "/1mgm.sf2"); // Fallback
          }
      }
      if(!sf2) sf2 = new AudioFileSourceLittleFS(LittleFS, "/1mgm.sf2");
      if(!sf2) {
          Serial.println("[Audio] Soundfont não encontrado");
          return false;
      }
      AudioGeneratorMIDI* midi = new AudioGeneratorMIDI();
      midi->SetSoundfont(sf2);
      generator = midi;
    }
    if (generator && source && audioout) {
        Serial.println("Start audio");
        generator->begin(source, audioout);
        while (generator->isRunning()) {
            if (!generator->loop() || check(AnyKeyPress)) generator->stop();
        }
        audioout->stop();
        source->close();
        Serial.println("Stop audio");

        delete generator;
        delete source;
        delete audioout;

        return true;
    }
    // else
    return false; // init error
}

bool playAudioRTTTLString(String song) {
    if (!leleConfig.soundEnabled) return false;

    // derived from
    // https://github.com/earlephilhower/ESP8266Audio/blob/master/examples/PlayRTTTLToI2SDAC/PlayRTTTLToI2SDAC.ino

    song.trim();
    if (song == "") return false;

    AudioOutputI2S *audioout = new AudioOutputI2S();
    audioout->SetPinout(BCLK, WCLK, DOUT, MCLK);

    AudioGenerator *generator = new AudioGeneratorRTTTL();

    AudioFileSource *source = new AudioFileSourcePROGMEM(song.c_str(), song.length());

    if (generator && source && audioout) {
        Serial.println("Start audio");
        generator->begin(source, audioout);
        while (generator->isRunning()) {
            if (!generator->loop() || check(AnyKeyPress)) generator->stop();
        }
        audioout->stop();
        source->close();
        Serial.println("Stop audio");

        delete generator;
        delete source;
        delete audioout;

        return true;
    }
    // else
    return false; // init error
}

bool tts(String text) {
    if (!leleConfig.soundEnabled) return false;

    text.trim();
    if (text == "") return false;

    AudioOutputI2S *audioout = new AudioOutputI2S();
    audioout->SetPinout(BCLK, WCLK, DOUT, MCLK);

    // https://github.com/earlephilhower/ESP8266SAM/blob/master/examples/Speak/Speak.ino
    audioout->begin();
    ESP8266SAM *sam = new ESP8266SAM;
    sam->Say(audioout, text.c_str());
    delete sam;
    delete audioout;
    return true;
}

bool isAudioFile(String filepath) {

    return filepath.endsWith(".opus") || filepath.endsWith(".rtttl") || filepath.endsWith(".wav") ||
           filepath.endsWith(".mod") || filepath.endsWith(".mp3");
}

void playTone(unsigned int frequency, unsigned long duration, short waveType) {
    if (!leleConfig.soundEnabled) return;

    // derived from
    // https://github.com/earlephilhower/ESP8266Audio/blob/master/examples/PlayWAVFromFunction/PlayWAVFromFunction.ino

    if (frequency == 0 || duration == 0) return;

    float hz = frequency;

    AudioGeneratorWAV *wav;
    AudioFileSourceFunction *file;
    AudioOutputI2S *out = new AudioOutputI2S();
    out->SetPinout(BCLK, WCLK, DOUT, MCLK);

    file = new AudioFileSourceFunction(duration / 1000.0); // , 1, 44100
    //
    // you can set (sec, channels, hz, bit/sample) but you should care about
    // the trade-off between performance and the audio quality
    //
    // file = new AudioFileSourceFunction(sec, channels, hz, bit/sample);
    // channels   : default = 1
    // hz         : default = 8000 (8000, 11025, 22050, 44100, 48000, etc.)
    // bit/sample : default = 16 (8, 16, 32)

    // ===== set your sound function =====

    if (waveType == 0) { // square
        file->addAudioGenerators([&](const float time) {
            float v = (sin(hz * time) >= 0) ? 1.0f : -1.0f;
            ;         // generate square wave
            v *= 0.1; // scale
            return v;
        });
    } else if (waveType == 1) { // sine
        file->addAudioGenerators([&](const float time) {
            float v = sin(TWO_PI * hz * time); // generate sine wave
            v *= fmod(time, 1.f);              // change linear
            v *= 0.1;                          // scale
            return v;
        });
    }
    } else if (waveType == 2) { // triangle
        file->addAudioGenerators([&](const float time) {
            float t = fmod(hz * time, 1.0f);
            float v = 4.0f * fabs(t - 0.5f) - 1.0f; // Onda triângulo
            v *= 0.1; // scale
            return v;
        });
    } else if (waveType == 3) { // sawtooth
        file->addAudioGenerators([&](const float time) {
            float t = fmod(hz * time, 1.0f);
            float v = 2.0f * t - 1.0f; // Onda dente de serra
            v *= 0.1; // scale
            return v;
        });
    }
    // Wave types: 0=square, 1=sine, 2=triangle, 3=sawtooth
    //
    // sound function should have one argument(float) and one return(float)
    // param  : float (current time [sec] of the song)
    // return : float (the amplitude of sound which varies from -1.f to +1.f)

    wav = new AudioGeneratorWAV();
    wav->begin(file, out);

    while (wav->isRunning()) {
        if (!wav->loop() || check(AnyKeyPress)) wav->stop();
    }

    delete file;
    delete wav;
    delete out;
}

#endif

void _tone(unsigned int frequency, unsigned long duration) {
    if (!leleConfig.soundEnabled) return;

#if defined(BUZZ_PIN)
    tone(BUZZ_PIN, frequency, duration);
#elif defined(HAS_NS4168_SPKR)
    //  alt. implementation using the speaker
    playTone(frequency, duration, 0);
#endif
}
`nbool think_and_speak(String phrase) {`n    if (!leleConfig.soundEnabled) return false;`n`n    phrase.trim();`n    if (phrase == "") return false;`n`n    // Enable PAM8403 amplifier for TTS`n    #ifdef HAS_PAM8403`n    digitalWrite(PAM8403_EN_PIN, HIGH);`n    delay(10); // Allow amplifier to stabilize`n    #endif`n`n    // Configure I2S for TTS if not already configured`n    static bool i2s_initialized = false;`n    if (!i2s_initialized) {`n        i2s_config_t i2s_cfg = {`n            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),`n            .sample_rate = 22050,`n            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,`n            .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,`n            .communication_format = I2S_COMM_FORMAT_I2S,`n            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,`n            .dma_buf_count = 6,`n            .dma_buf_len = 512,`n            .use_apll = false,`n            .tx_desc_auto_clear = true,`n            .fixed_mclk = 0`n        };`n        `n        i2s_pin_config_t pin_cfg = {`n            .bck_io_num = BCLK,`n            .ws_io_num = WCLK,`n            .data_out_num = DOUT,`n            .data_in_num = I2S_PIN_NO_CHANGE`n        };`n        `n        if (i2s_driver_install(I2S_NUM_0, &i2s_cfg, 0, NULL) == ESP_OK &&`n            i2s_set_pin(I2S_NUM_0, &pin_cfg) == ESP_OK) {`n            i2s_initialized = true;`n        }`n    }`n`n    // Use ESP8266SAM with Portuguese phonetics`n    AudioOutputI2S *audioout = new AudioOutputI2S();`n    audioout->SetPinout(BCLK, WCLK, DOUT, MCLK);`n    audioout->begin();`n    `n    ESP8266SAM *sam = new ESP8266SAM;`n    `n    // Configure for Brazilian Portuguese speech`n    sam->SetVoice(ESP8266SAM::VOICE::SPANISH); // Closest to Portuguese`n    sam->SetSpeed(80);  // Slightly slower for clarity`n    sam->SetPitch(64);  // Natural pitch`n    sam->SetMouth(128); // Natural mouth shape`n    sam->SetThroat(128); // Natural throat`n    `n    // Speak the phrase`n    sam->Say(audioout, phrase.c_str());`n    `n    delete sam;`n    delete audioout;`n`n    // Update last speech timestamp for deep sleep logic`n    extern unsigned long last_speech_time;`n    last_speech_time = millis\(\);`n`n    // Disable amplifier to save power`n    #ifdef HAS_PAM8403`n    digitalWrite\(PAM8403_EN_PIN, LOW\);`n    #endif`n`n    return true;`n\}
`nbool init_tts_system() {`n    // Initialize I2S for TTS on ESP32-S3`n    #ifdef CYD_S3`n    i2s_config_t i2s_cfg = {`n        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),`n        .sample_rate = 22050,`n        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,`n        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,`n        .communication_format = I2S_COMM_FORMAT_I2S,`n        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,`n        .dma_buf_count = 6,`n        .dma_buf_len = 512,`n        .use_apll = false,`n        .tx_desc_auto_clear = true,`n        .fixed_mclk = 0`n    };`n    `n    i2s_pin_config_t pin_cfg = {`n        .bck_io_num = I2S_BCLK_PIN,`n        .ws_io_num = I2S_LRCK_PIN,`n        .data_out_num = I2S_DOUT_PIN,`n        .data_in_num = I2S_PIN_NO_CHANGE`n    };`n    `n    esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2s_cfg, 0, NULL);`n    if (err != ESP_OK) {`n        log_e("[TTS] I2S driver install failed: %d", err);`n        return false;`n    }`n    `n    err = i2s_set_pin(I2S_NUM_0, &pin_cfg);`n    if (err != ESP_OK) {`n        log_e("[TTS] I2S set pin failed: %d", err);`n        return false;`n    }`n    `n    log_i("[TTS] I2S initialized for PCM5102A DAC");`n    return true;`n    #else`n    return true; // No special initialization needed for other boards`n    #endif`n}

