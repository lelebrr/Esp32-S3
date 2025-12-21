#include "attacks_manager.h"
#include "pin_config.h"
#include "rf_core.h"
#include "s3_driver.h"

// External Dependencies
#include <NimBLEDevice.h>
#include <WiFi.h>
#include <esp_wifi.h>
// #include <IRremoteESP8266.h> // Removed for YS-IRTM
// #include <IRsend.h>          // Removed for YS-IRTM

// Internal Drivers
#include "gps_driver.h"
#include "sd_adapter.h"

// ============================================================================
// GLOBALS
// ============================================================================
static AttackType current_attack = ATTACK_NONE;
static TaskHandle_t attack_task_handle = NULL;
static volatile bool stop_requested = false;

// IR Object (YS-IRTM)
static YsIrtm irDriver(PIN_YS_IR_RX, PIN_YS_IR_TX);

// ============================================================================
// HELPER WRAPPERS
// ============================================================================
static void set_module_power(int mod_type) {
    // 0=None, 1=CC1101, 2=PN532, 3=BLE
    bool cc = (mod_type == 1);
    bool nfc = (mod_type == 2);
    bool ble = (mod_type == 3);
    MonsterDriver::powerOnModules(false, ble, nfc, cc);
}

// ============================================================================
// TASK DEFINITIONS
// ============================================================================

// --- BLE SPAM TASK ---
static void task_ble_spam(void *pvParameters) {
    Serial.println("[ATTACK] Starting BLE Spam...");
    set_module_power(3);
    NimBLEDevice::init("MonsterS3");
    NimBLEServer *pServer = NimBLEDevice::createServer();
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->start();

    while (!stop_requested) { delay(100); }

    NimBLEDevice::deinit();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- WIFI DEAUTH TASK ---
static void task_wifi_deauth(void *pvParameters) {
    Serial.println("[ATTACK] Starting WiFi Deauth...");
    WiFi.mode(WIFI_MODE_STA);
    esp_wifi_set_promiscuous(true);

    while (!stop_requested) { delay(100); }

    WiFi.mode(WIFI_OFF);
    vTaskDelete(NULL);
}

// --- WIFI WARDRIVING TASK ---
// Scans networks and logs with GPS coordinates
static void task_wifi_wardrive(void *pvParameters) {
    Serial.println("[ATTACK] Starting WiFi Wardriving...");
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();

    // Create wardriving log file
    char filename[32];
    snprintf(filename, sizeof(filename), "/wardrive_%lu.csv", millis());

    // Header
    SDAdapter::appendFile(SD, filename, "Date,Time,Lat,Lon,Alt,HDOP,SSID,BSSID,RSSI,Channel,Encrypt\n");

    while (!stop_requested) {
        int n = WiFi.scanNetworks(false, true); // Async=false, Hidden=true

        if (n > 0) {
            // Get GPS Data once for this batch
            double lat = GPSDriver::getLatitude();
            double lon = GPSDriver::getLongitude();
            String date = GPSDriver::getDateString();
            String time = GPSDriver::getTimeString();
            double alt = GPSDriver::getAltitude();
            double hdop = GPSDriver::getHDOP();

            for (int i = 0; i < n; ++i) {
                // Log to CSV
                char logLine[256];
                snprintf(
                    logLine,
                    sizeof(logLine),
                    "%s,%s,%.6f,%.6f,%.1f,%.1f,%s,%s,%d,%d,%d\n",
                    date.c_str(),
                    time.c_str(),
                    lat,
                    lon,
                    alt,
                    hdop,
                    WiFi.SSID(i).c_str(),
                    WiFi.BSSIDstr(i).c_str(),
                    WiFi.RSSI(i),
                    WiFi.channel(i),
                    WiFi.encryptionType(i)
                );

                SDAdapter::appendFile(SD, filename, logLine);
                Serial.printf("[WARDRIVE] Logged: %s (%d dBm)\n", WiFi.SSID(i).c_str(), WiFi.RSSI(i));
            }
        }

        WiFi.scanDelete();
        delay(100); // Small pause between scans
    }

    WiFi.mode(WIFI_OFF);
    vTaskDelete(NULL);
}

// --- RF JAMMER CONTINUOUS 433 ---
static void task_rf_jammer_433(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Jammer 433MHz...");
    set_module_power(1);

    RFCore::startJammerContinuous(FREQ_433);

    while (!stop_requested) { RFCore::updateJammer(); }

    RFCore::stopJammer();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF JAMMER 315 ---
static void task_rf_jammer_315(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Jammer 315MHz...");
    set_module_power(1);

    RFCore::startJammerContinuous(FREQ_315);

    while (!stop_requested) { RFCore::updateJammer(); }

    RFCore::stopJammer();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF JAMMER 868 ---
static void task_rf_jammer_868(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Jammer 868MHz...");
    set_module_power(1);

    RFCore::startJammerContinuous(FREQ_868);

    while (!stop_requested) { RFCore::updateJammer(); }

    RFCore::stopJammer();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF JAMMER BURST ---
static void task_rf_jammer_burst(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Jammer BURST...");
    set_module_power(1);

    RFCore::startJammerBurst(FREQ_433, 100, 50);

    while (!stop_requested) { RFCore::updateJammer(); }

    RFCore::stopJammer();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF JAMMER SMART ---
static void task_rf_jammer_smart(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Jammer SMART...");
    set_module_power(1);

    RFCore::startJammerSmart(FREQ_433);

    while (!stop_requested) { RFCore::updateJammer(); }

    RFCore::stopJammer();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF CAPTURE ---
static void task_rf_capture(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Capture...");
    set_module_power(1);

    RFCore::startReceive(FREQ_433);

    while (!stop_requested) {
        if (RFCore::hasSignal()) {
            CapturedSignal sig = RFCore::getLastCapture();
            Serial.printf(
                "[RF] Captured: %d bytes, RSSI: %d, Proto: %s\n",
                sig.rawLength,
                sig.rssi,
                RFCore::getProtocolName(sig.protocol)
            );

            // Auto-save to SD
            char filename[32];
            snprintf(filename, sizeof(filename), "/subghz/capture_%lu.txt", millis());
            RFCore::saveSignal(filename, &sig);
        }
        delay(10);
    }

    RFCore::stopReceive();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF REPLAY ---
static void task_rf_replay(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Replay...");
    set_module_power(1);

    if (!RFCore::replayLast()) { Serial.println("[ATTACK] No signal to replay!"); }

    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF GHOST REPLAY ---
static void task_rf_ghost(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Ghost Replay...");
    set_module_power(1);

    RFCore::startGhostReplay(8000, 45000, 10);

    while (!stop_requested && RFCore::isGhostActive()) {
        // Ghost timing handled internally
        CapturedSignal sig = RFCore::getLastCapture();
        if (sig.valid) {
            uint32_t now = millis();
            // Check if it's time for next replay
            delay(100);
        }
    }

    RFCore::stopGhostReplay();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF BRUTE FORCE ---
static void task_rf_brute(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Brute Force...");
    set_module_power(1);

    RFCore::startBruteForce(FREQ_433, 12, PROTO_CAME); // 12-bit CAME codes

    while (!stop_requested && RFCore::isBruteForcing()) {
        RFCore::updateBruteForce();

        if (RFCore::getBruteProgress() % 100 == 0) {
            Serial.printf(
                "[RF] Brute: %lu/%lu (%.1f%%)\n",
                RFCore::getBruteProgress(),
                RFCore::getBruteTotal(),
                (float)RFCore::getBruteProgress() / RFCore::getBruteTotal() * 100
            );
        }
    }

    RFCore::stopBruteForce();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF SPECTRUM ANALYZER ---
static void task_rf_spectrum(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Spectrum Analyzer...");
    set_module_power(1);

    RFCore::startFrequencyScan(430.0f, 440.0f, 0.1f);

    while (!stop_requested) {
        RFCore::updateScanner();

        SpectrumData data = RFCore::getSpectrumData();
        float strongest = RFCore::findStrongestFrequency();

        Serial.printf("[RF] Scan: Strongest @ %.2f MHz\n", strongest);
        delay(500);
    }

    RFCore::stopFrequencyScan();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- IR TV OFF (NEC ONLY) ---
static void task_ir_tv_off(void *pvParameters) {
    Serial.println("[ATTACK] Starting IR TV-B-Gone (NEC Only)...");
    irDriver.begin();

    // Common NEC TV Power Codes
    // Struct: { UserCode, CommandCode }
    struct IrCode {
        uint16_t user;
        uint8_t cmd;
    };

    const IrCode codes[] = {
        {0x20DF, 0x10}, // LG Power (User: 20DF, Cmd: 10)
        {0x0707, 0x02}, // Generic NEC
        {0x0000, 0x12}, // Example
        // Note: Samsung/Sony are NOT NEC, so they are removed
    };

    while (!stop_requested) {
        for (int i = 0; i < 3 && !stop_requested; i++) {
            irDriver.sendNEC(codes[i].user, codes[i].cmd);
            delay(100);
        }
        delay(1000);
    }

    irDriver.end();
    vTaskDelete(NULL);
}

// --- IR CLONE (NEC) ---
static void task_ir_clone(void *pvParameters) {
    Serial.println("[ATTACK] Starting IR Cloner...");
    irDriver.begin();

    while (!stop_requested) {
        uint16_t userCode;
        uint8_t cmdCode;

        if (irDriver.read(&userCode, &cmdCode)) {
            Serial.printf("[IR] Captured: User=0x%04X, Cmd=0x%02X\n", userCode, cmdCode);

            // Save to SD
            char filename[64];
            snprintf(filename, sizeof(filename), "/ir_capture.txt"); // Simple append
            char logLine[64];
            snprintf(logLine, sizeof(logLine), "NEC,0x%04X,0x%02X\n", userCode, cmdCode);
            SDAdapter::appendFile(SD, filename, logLine);

            // Blink LED or feedback?
        }
        delay(10);
    }

    irDriver.end();
    vTaskDelete(NULL);
}

// ============================================================================
// MANAGER FUNCTIONS
// ============================================================================

void attacks_init() {
    stop_requested = false;
    current_attack = ATTACK_NONE;
    attack_task_handle = NULL;

    // RF Core disabled - uncomment when CC1101 is connected
    // RFCore::init();
    Serial.println("[ATTACKS] RF disabled - CC1101 not connected");

    Serial.println("[ATTACKS] Manager Initialized");
}

void attacks_stop() {
    if (current_attack == ATTACK_NONE) return;

    stop_requested = true;
    delay(200);

    if (attack_task_handle != NULL) {
        vTaskDelete(attack_task_handle);
        attack_task_handle = NULL;
    }

    // Stop any active RF operations
    RFCore::stopJammer();
    RFCore::stopReceive();
    RFCore::stopGhostReplay();
    RFCore::stopBruteForce();
    RFCore::stopFrequencyScan();

    set_module_power(0);
    current_attack = ATTACK_NONE;

    Serial.println("[ATTACKS] Stopped");
}

void attacks_start(AttackType type) {
    if (current_attack == type) return;

    attacks_stop();
    stop_requested = false;
    current_attack = type;

    Serial.printf("[ATTACKS] Starting: %s\n", attacks_get_name(type));

    switch (type) {
        // BLE
        case ATTACK_BLE_SPAM:
        case ATTACK_BLE_SOUR_APPLE:
        case ATTACK_BLE_SWIFT_PAIR:
        case ATTACK_BLE_FAST_PAIR:
            xTaskCreate(task_ble_spam, "Att_BLE", 4096, NULL, 3, &attack_task_handle);
            break;

        // WiFi
        case ATTACK_WIFI_DEAUTH:
        case ATTACK_WIFI_BEACON_SPAM:
            xTaskCreate(task_wifi_deauth, "Att_WiFi", 4096, NULL, 3, &attack_task_handle);
            break;

        // RF Jammers
        case ATTACK_RF_JAMMER_433:
            xTaskCreate(task_rf_jammer_433, "Att_RF", 4096, NULL, 5, &attack_task_handle);
            break;
        case ATTACK_RF_JAMMER_315:
            xTaskCreate(task_rf_jammer_315, "Att_RF", 4096, NULL, 5, &attack_task_handle);
            break;
        case ATTACK_RF_JAMMER_868:
            xTaskCreate(task_rf_jammer_868, "Att_RF", 4096, NULL, 5, &attack_task_handle);
            break;
        case ATTACK_RF_JAMMER_BURST:
            xTaskCreate(task_rf_jammer_burst, "Att_RF", 4096, NULL, 5, &attack_task_handle);
            break;
        case ATTACK_RF_JAMMER_SMART:
            xTaskCreate(task_rf_jammer_smart, "Att_RF", 4096, NULL, 5, &attack_task_handle);
            break;

        // RF Capture/Replay
        case ATTACK_RF_CAPTURE:
        case ATTACK_RF_SCAN:
            xTaskCreate(task_rf_capture, "Att_RF", 4096, NULL, 4, &attack_task_handle);
            break;
        case ATTACK_RF_REPLAY:
            xTaskCreate(task_rf_replay, "Att_RF", 4096, NULL, 4, &attack_task_handle);
            break;
        case ATTACK_RF_GHOST_REPLAY:
            xTaskCreate(task_rf_ghost, "Att_RF", 4096, NULL, 4, &attack_task_handle);
            break;

        // RF Brute Force
        case ATTACK_RF_BRUTE_FORCE:
        case ATTACK_RF_DEBRUIJN:
            xTaskCreate(task_rf_brute, "Att_RF", 4096, NULL, 4, &attack_task_handle);
            break;

        // RF Spectrum
        case ATTACK_RF_SPECTRUM:
            xTaskCreate(task_rf_spectrum, "Att_RF", 4096, NULL, 3, &attack_task_handle);
            break;

        // IR
        case ATTACK_IR_TV_OFF:
            xTaskCreate(task_ir_tv_off, "Att_IR", 4096, NULL, 2, &attack_task_handle);
            break;
        case ATTACK_IR_BRUTE:
            // Reuse TV OFF logic or create brute force task
            // For now mapping to TV off or unimplemented
            current_attack = ATTACK_NONE;
            break;
        case ATTACK_IR_CLONE: xTaskCreate(task_ir_clone, "Att_IR", 4096, NULL, 2, &attack_task_handle); break;

        default: current_attack = ATTACK_NONE; break;
    }
}

bool attacks_is_running() { return current_attack != ATTACK_NONE; }

AttackType attacks_get_current() { return current_attack; }

void attacks_update() {
    // Called from main loop for time-based updates
    // Most logic is in dedicated tasks now
}

void attacks_report_success() { Serial.println("[ATTACKS] Success reported"); }

void attacks_report_fail() { Serial.println("[ATTACKS] Fail reported"); }

const char *attacks_get_name(AttackType type) {
    switch (type) {
        case ATTACK_NONE: return "None";

        // BLE
        case ATTACK_BLE_SPAM: return "BLE Spam";
        case ATTACK_BLE_SOUR_APPLE: return "Sour Apple";
        case ATTACK_BLE_SWIFT_PAIR: return "Swift Pair";
        case ATTACK_BLE_FAST_PAIR: return "Fast Pair";

        // WiFi
        case ATTACK_WIFI_DEAUTH: return "WiFi Deauth";
        case ATTACK_WIFI_BEACON_SPAM: return "Beacon Spam";
        case ATTACK_WIFI_EVIL_TWIN: return "Evil Twin";
        case ATTACK_WIFI_PMKID: return "PMKID Capture";
        case ATTACK_WIFI_WARDRIVE: return "Wardriving";

        // RF
        case ATTACK_RF_JAMMER_433: return "Jammer 433MHz";
        case ATTACK_RF_JAMMER_315: return "Jammer 315MHz";
        case ATTACK_RF_JAMMER_868: return "Jammer 868MHz";
        case ATTACK_RF_JAMMER_BURST: return "Jammer Burst";
        case ATTACK_RF_JAMMER_SMART: return "Jammer Smart";
        case ATTACK_RF_SCAN: return "RF Scan";
        case ATTACK_RF_CAPTURE: return "RF Capture";
        case ATTACK_RF_REPLAY: return "RF Replay";
        case ATTACK_RF_GHOST_REPLAY: return "Ghost Replay";
        case ATTACK_RF_BRUTE_FORCE: return "RF Brute Force";
        case ATTACK_RF_DEBRUIJN: return "De Bruijn";
        case ATTACK_RF_SPECTRUM: return "Spectrum Analyzer";

        // NFC
        case ATTACK_NFC_CLONE: return "NFC Clone";
        case ATTACK_NFC_FAULT: return "NFC Fault";
        case ATTACK_NFC_PHISHING: return "NFC Phishing";

        // IR
        case ATTACK_IR_BRUTE: return "IR Brute";
        case ATTACK_IR_TV_OFF: return "TV-B-Gone (NEC)";
        case ATTACK_IR_CLONE: return "IR Cloner";

        // USB
        case ATTACK_USB_BADUSB: return "BadUSB";
        case ATTACK_USB_EXFIL: return "USB Exfil";

        default: return "Unknown";
    }
}
