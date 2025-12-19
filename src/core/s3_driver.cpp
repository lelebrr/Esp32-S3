// ESP32-S3 CYD N16R8 Interface Implementation
// Moved from boards/CYD-S3-N16R8/interface.cpp to fix dependency issues

#include <globals.h>
#include "utils.h"
#include <Arduino.h>
#include <interface.h>

// Touch Controller (XPT2046 via SPI shared with Display)
#include "CYD28_TouchscreenR.h"
#define CYD28_DISPLAY_HOR_RES_MAX 480
#define CYD28_DISPLAY_VER_RES_MAX 320
CYD28_TouchR touch(CYD28_DISPLAY_HOR_RES_MAX, CYD28_DISPLAY_VER_RES_MAX);

// XPT2046 CS pin
#ifndef TOUCH_CS
#define TOUCH_CS 3
#endif

// Hardware detection flags (set by s3_hardware_init.cpp)
bool rtc_detected = false;
bool gesture_detected = false;
bool nfc_detected = false;
uint8_t ds18b20_count = 0;

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   ESP32-S3 specific GPIO initialization
***************************************************************************************/
void _setup_gpio() {
    // XPT2046 Touch CS
    pinMode(TOUCH_CS, OUTPUT);
    digitalWrite(TOUCH_CS, HIGH);

    // Initialize touch controller
    if (!touch.begin()) {
        Serial.println("[S3] Touch IC not started");
        log_i("[S3] Touch IC not started");
    } else {
        log_i("[S3] Touch IC started successfully");
    }

    leleConfig.colorInverted = 0;
}

/***************************************************************************************
** Function name: _post_setup_gpio()
** Location: main.cpp
** Description:   Second stage GPIO setup for S3
***************************************************************************************/
void _post_setup_gpio() {
    // Backlight PWM setup using LEDC for S3
    #ifdef TFT_BL
    pinMode(TFT_BL, OUTPUT);
    ledcAttach(TFT_BL, TFT_BRIGHT_FREQ, TFT_BRIGHT_Bits);
    ledcWrite(TFT_BL, 255);
    #endif
    
    // PSRAM check
    if (psramFound()) {
        log_i("[S3] PSRAM detected: %d bytes", ESP.getPsramSize());
        log_i("[S3] Free PSRAM: %d bytes", ESP.getFreePsram());
    } else {
        log_w("[S3] PSRAM not detected! Check OPI configuration.");
    }
    
    // Print S3 summary
    log_i("[S3] ESP32-S3 @ %dMHz ready", getCpuFrequencyMhz());
}

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value (S3 uses LEDC differently)
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    int dutyCycle;
    if (brightval == 100) dutyCycle = 255;
    else if (brightval == 75) dutyCycle = 130;
    else if (brightval == 50) dutyCycle = 70;
    else if (brightval == 25) dutyCycle = 20;
    else if (brightval == 0) dutyCycle = 0;
    else dutyCycle = ((brightval * 255) / 100);

    #ifdef TFT_BL
    ledcWrite(TFT_BL, dutyCycle);
    #endif
}

/*********************************************************************
** Function: InputHandler
** Handles touch input for S3
**********************************************************************/
void InputHandler(void) {
    static long d_tmp = 0;
    
    if (millis() - d_tmp > 200 || LongPress) {
        if (touch.touched()) {
            auto t = touch.getPointScaled();
            
            // Rotation handling
            if (leleConfig.rotation == 3) {
                t.y = (tftHeight + 20) - t.y;
                t.x = tftWidth - t.x;
            }
            if (leleConfig.rotation == 0) {
                int tmp = t.x;
                t.x = tftWidth - t.y;
                t.y = tmp;
            }
            if (leleConfig.rotation == 2) {
                int tmp = t.x;
                t.x = t.y;
                t.y = (tftHeight + 20) - tmp;
            }

            if (!wakeUpScreen()) AnyKeyPress = true;
            else goto END;

            touchPoint.x = t.x;
            touchPoint.y = t.y;
            touchPoint.pressed = true;
            touchHeatMap(touchPoint);
        END:
            d_tmp = millis();
        }
    }
}

/*********************************************************************
** Function: powerOff
** ESP32-S3 deep sleep with multiple wake sources
**********************************************************************/
void powerOff() {
    // Configure ext1 wake (multiple GPIO sources)
    esp_sleep_enable_ext1_wakeup(
        (1ULL << 0) | (1ULL << 8),  // GPIO0 (button) + GPIO8 (touch IRQ)
        ESP_EXT1_WAKEUP_ANY_LOW
    );
    
    log_i("[S3] Entering deep sleep...");
    esp_deep_sleep_start();
}

/*********************************************************************
** Function: checkReboot
** ESP32-S3 reboot logic
**********************************************************************/
void checkReboot() {
    // Can be used for button combo detection
}
