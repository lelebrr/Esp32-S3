/**
 * @file usb_driver.h
 * @brief USB HID Attack Driver for Monster S3 Firmware
 * 
 * ESP32-S3 has native USB OTG support for:
 * - USB HID Keyboard (BadUSB attacks)
 * - USB Mass Storage (Exfiltration)
 * 
 * Note: USB CDC is used for Serial, so HID uses the OTG port
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#ifndef USB_DRIVER_H
#define USB_DRIVER_H

#include <Arduino.h>

#if CONFIG_IDF_TARGET_ESP32S3
#include "USB.h"
#include "USBHIDKeyboard.h"
#endif

// ============================================================================
// PAYLOAD STRUCTURE
// ============================================================================
struct USBPayload {
    const char* name;
    const char* script;      // DuckyScript-like commands
    uint16_t delayMs;
};

// ============================================================================
// USB DRIVER CLASS
// ============================================================================
class USBDriver {
public:
    /**
     * @brief Initialize USB HID subsystem
     * @return true if USB HID available
     */
    static bool init();
    
    /**
     * @brief Check if USB HID is ready
     */
    static bool isReady();
    
    /**
     * @brief Start USB HID keyboard mode
     */
    static void startKeyboard();
    
    /**
     * @brief Stop USB HID and return to normal mode
     */
    static void stopKeyboard();
    
    /**
     * @brief Type a string as keyboard input
     * @param text Text to type
     */
    static void type(const char* text);
    
    /**
     * @brief Type a string with delay between keys
     * @param text Text to type
     * @param delayMs Delay between keystrokes
     */
    static void typeWithDelay(const char* text, uint16_t delayMs);
    
    /**
     * @brief Press a key combination
     * @param key Key code
     * @param modifier Modifier (CTRL, ALT, SHIFT, GUI)
     */
    static void pressKey(uint8_t key, uint8_t modifier = 0);
    
    /**
     * @brief Execute a DuckyScript-like payload
     * @param script Payload script
     */
    static void executePayload(const char* script);
    
    /**
     * @brief Execute payload from SD card
     * @param filename Payload file on SD card
     */
    static bool executePayloadFromFile(const char* filename);
    
    // ========================================
    // Predefined Payloads
    // ========================================
    
    /**
     * @brief Open Run dialog (Windows)
     */
    static void openRunDialog();
    
    /**
     * @brief Open Terminal (Windows/Mac/Linux)
     */
    static void openTerminal();
    
    /**
     * @brief Execute PowerShell command
     * @param command PowerShell command
     */
    static void executePowerShell(const char* command);
    
    /**
     * @brief Download and execute payload
     * @param url URL to download from
     */
    static void downloadAndExecute(const char* url);
    
    /**
     * @brief Reverse shell payload
     * @param ip Attacker IP
     * @param port Attacker port
     */
    static void reverseShell(const char* ip, uint16_t port);
    
    /**
     * @brief WiFi credential exfiltration
     */
    static void exfilWiFiCredentials();
    
    /**
     * @brief SAM database dump (requires admin)
     */
    static void dumpSAM();

private:
    static bool _initialized;
    static bool _keyboardActive;
    
    #if CONFIG_IDF_TARGET_ESP32S3
    static USBHIDKeyboard _keyboard;
    #endif
    
    static void parseAndExecuteLine(const char* line);
};

// ============================================================================
// KEY CODES (USB HID) - Only define if not already defined by USBHIDKeyboard.h
// ============================================================================
#ifndef KEY_LEFT_CTRL
#define KEY_LEFT_CTRL   0x80
#endif
#ifndef KEY_LEFT_SHIFT
#define KEY_LEFT_SHIFT  0x81
#endif
#ifndef KEY_LEFT_ALT
#define KEY_LEFT_ALT    0x82
#endif
#ifndef KEY_LEFT_GUI
#define KEY_LEFT_GUI    0x83  // Windows/Command key
#endif
#ifndef KEY_RIGHT_CTRL
#define KEY_RIGHT_CTRL  0x84
#endif
#ifndef KEY_RIGHT_SHIFT
#define KEY_RIGHT_SHIFT 0x85
#endif
#ifndef KEY_RIGHT_ALT
#define KEY_RIGHT_ALT   0x86
#endif
#ifndef KEY_RIGHT_GUI
#define KEY_RIGHT_GUI   0x87
#endif

// Navigation keys (use Arduino USB library values)
#ifndef KEY_RETURN
#define KEY_RETURN      '\n'
#endif
#ifndef KEY_ESC
#define KEY_ESC         0xB1
#endif
#ifndef KEY_BACKSPACE
#define KEY_BACKSPACE   0xB2
#endif
#ifndef KEY_TAB
#define KEY_TAB         '\t'
#endif
#ifndef KEY_DELETE
#define KEY_DELETE      0xD4
#endif
#ifndef KEY_INSERT
#define KEY_INSERT      0xD1
#endif
#ifndef KEY_HOME
#define KEY_HOME        0xD2
#endif
#ifndef KEY_END
#define KEY_END         0xD5
#endif
#ifndef KEY_PAGE_UP
#define KEY_PAGE_UP     0xD3
#endif
#ifndef KEY_PAGE_DOWN
#define KEY_PAGE_DOWN   0xD6
#endif
#ifndef KEY_UP
#define KEY_UP          0xDA
#endif
#ifndef KEY_DOWN
#define KEY_DOWN        0xD9
#endif
#ifndef KEY_LEFT
#define KEY_LEFT        0xD8
#endif
#ifndef KEY_RIGHT
#define KEY_RIGHT       0xD7
#endif
#ifndef KEY_F1
#define KEY_F1          0xC2
#endif
#ifndef KEY_F2
#define KEY_F2          0xC3
#endif
#ifndef KEY_F3
#define KEY_F3          0xC4
#endif
#ifndef KEY_F4
#define KEY_F4          0xC5
#endif
#ifndef KEY_F5
#define KEY_F5          0xC6
#endif
#ifndef KEY_F6
#define KEY_F6          0xC7
#endif
#ifndef KEY_F7
#define KEY_F7          0xC8
#endif
#ifndef KEY_F8
#define KEY_F8          0xC9
#endif
#ifndef KEY_F9
#define KEY_F9          0xCA
#endif
#ifndef KEY_F10
#define KEY_F10         0xCB
#endif
#ifndef KEY_F11
#define KEY_F11         0xCC
#endif
#ifndef KEY_F12
#define KEY_F12         0xCD
#endif

#endif // USB_DRIVER_H

