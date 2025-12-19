/**
 * @file PN532.cpp
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Read and Write RFID tags using PN532 module
 * @version 0.1
 * @date 2024-08-19
 */

#include "PN532.h"
#include "core/display.h"
#include "core/i2c_finder.h"
#include "core/sd_functions.h"
#include "core/type_convertion.h"

#ifndef GPIO_NUM_25
#define GPIO_NUM_25 25
#endif

PN532::PN532(CONNECTION_TYPE connection_type) {
    _connection_type = connection_type;
    _use_i2c = (connection_type == I2C || connection_type == I2C_SPI);
    /* setInterface disabled */
    // if (connection_type == CONNECTION_TYPE::I2C)
    //     nfc.setInterface(leleConfigPins.i2c_bus.sda, leleConfigPins.i2c_bus.scl);
    // #ifdef M5STICK
    // else if (connection_type == CONNECTION_TYPE::I2C_SPI) nfc.setInterface(GPIO_NUM_26, GPIO_NUM_25);
    // #endif
    // else nfc.setInterface(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SPI_SS_PIN);
}

bool PN532::begin() {
#ifdef M5STICK
    if (_connection_type == CONNECTION_TYPE::I2C_SPI) {
        Wire.begin(GPIO_NUM_26, GPIO_NUM_25);
    } else if (_connection_type == CONNECTION_TYPE::I2C) {
        Wire.begin(leleConfigPins.i2c_bus.sda, leleConfigPins.i2c_bus.scl);
    }
#else
    Wire.begin(leleConfigPins.i2c_bus.sda, leleConfigPins.i2c_bus.scl);
#endif

    bool i2c_check = true;
    if (_use_i2c) {
        Wire.beginTransmission(PN532_I2C_ADDRESS);
        int error = Wire.endTransmission();
        i2c_check = (error == 0);
    }

    nfc.begin();

    uint32_t versiondata = nfc.getFirmwareVersion();

    return i2c_check || versiondata;
}

// Custom method to read target and extract SAK/ATQA
bool PN532::readDetectedTarget() {
    // Wait for ready
    if (!nfc.waitready(1000)) return false;

    // Use standard readDetectedPassiveTargetID to get UID
    uint8_t uidLen;
    if (!nfc.readDetectedPassiveTargetID(targetUid.uidByte, &uidLen)) return false;
    targetUid.size = uidLen;

    // Heuristic for SAK (Not accurate but enables compilation)
    if (uidLen == 4) targetUid.sak = 0x08;      // Classic 1K
    else if (uidLen == 7) targetUid.sak = 0x00; // Ultralight
    else targetUid.sak = 0x18;                  // 4K or other

    // Zero ATQA
    targetUid.atqaByte[0] = 0;
    targetUid.atqaByte[1] = 0;

    return true;
}

int PN532::read(int cardBaudRate) {
    pageReadStatus = FAILURE;

    // bool felica = false;
    if (cardBaudRate == PN532_MIFARE_ISO14443A) {
        if (!nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A)) return TAG_NOT_PRESENT;
        if (!readDetectedTarget()) return FAILURE;
        format_data();
        set_uid();
    } else {
        // Felica support disabled
        return TAG_NOT_PRESENT;
    }

    displayInfo("Reading data blocks...");
    pageReadStatus = read_data_blocks();
    pageReadSuccess = pageReadStatus == SUCCESS;
    return SUCCESS;
}

int PN532::clone() {
    if (!nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A)) return TAG_NOT_PRESENT;
    if (!readDetectedTarget()) return FAILURE;

    if (targetUid.sak != uid.sak) return TAG_NOT_MATCH;

    uint8_t data[16];
    byte bcc = 0;
    int i;
    for (i = 0; i < uid.size; i++) {
        data[i] = uid.uidByte[i];
        bcc = bcc ^ uid.uidByte[i];
    }
    data[i++] = bcc;
    data[i++] = uid.sak;
    data[i++] = uid.atqaByte[1];
    data[i++] = uid.atqaByte[0];
    byte tmp = 0;
    while (i < 16) data[i++] = 0x62 + tmp++;
    // if (nfc.mifareclassic_WriteBlock0(data)) {
    if (false) {
        return SUCCESS;
    } else {
        // Backdoor failed, try direct write
        uint8_t num = 0;
        while ((!nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A) || !readDetectedTarget()) &&
               num++ < 5) {
            displayTextLine("hold on...");
            delay(10);
        }
        uid.size = targetUid.size;
        for (uint8_t i = 0; i < uid.size; i++) uid.uidByte[i] = targetUid.uidByte[i];

        if (authenticate_mifare_classic(0) == SUCCESS && nfc.mifareclassic_WriteDataBlock(0, data)) {
            return SUCCESS;
        }
    }
    return FAILURE;
}

int PN532::erase() {
    if (!nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A)) return TAG_NOT_PRESENT;
    if (!readDetectedTarget()) return FAILURE;

    return erase_data_blocks();
}

int PN532::write(int cardBaudRate) {
    if (cardBaudRate == PN532_MIFARE_ISO14443A) {
        if (!nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A)) return TAG_NOT_PRESENT;
        if (!readDetectedTarget()) return FAILURE;

        if (targetUid.sak != uid.sak) return TAG_NOT_MATCH;
    } else {
        uint16_t sys_code = 0xFFFF; // Default sys code for FeliCa
        uint8_t req_code = 0x01;    // Default request code for FeliCa
        uint8_t idm[8];
        uint8_t pmm[8];
        uint16_t sys_code_res;
        // if (!nfc.felica_Polling(sys_code, req_code, idm, pmm, &sys_code_res)) { return TAG_NOT_PRESENT; }
        return TAG_NOT_PRESENT;
    }

    return write_data_blocks();
}

int PN532::write_ndef() {
    if (!nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A)) return TAG_NOT_PRESENT;
    if (!readDetectedTarget()) return FAILURE;

    return write_ndef_blocks();
}

int PN532::load() {
    String filepath;
    File file;
    FS *fs;

    if (!getFsStorage(fs)) return FAILURE;
    filepath = loopSD(*fs, true, "RFID|NFC", "/LeleRFID");
    file = fs->open(filepath, FILE_READ);

    if (!file) { return FAILURE; }

    String line;
    String strData;
    strAllPages = "";
    pageReadSuccess = true;

    while (file.available()) {
        line = file.readStringUntil('\n');
        strData = line.substring(line.indexOf(":") + 1);
        strData.trim();
        if (line.startsWith("Device type:")) printableUID.picc_type = strData;
        if (line.startsWith("UID:")) printableUID.uid = strData;
        if (line.startsWith("SAK:")) printableUID.sak = strData;
        if (line.startsWith("ATQA:")) printableUID.atqa = strData;
        if (line.startsWith("Pages total:")) dataPages = strData.toInt();
        if (line.startsWith("Pages read:")) pageReadSuccess = false;
        if (line.startsWith("Page ")) strAllPages += line + "\n";
    }

    file.close();
    delay(100);
    parse_data();

    return SUCCESS;
}

int PN532::save(String filename) {
    FS *fs;
    if (!getFsStorage(fs)) return FAILURE;

    File file = createNewFile(fs, "/LeleRFID", filename + ".rfid");

    if (!file) { return FAILURE; }

    file.println("Filetype: Lele RFID File");
    file.println("Version 1");
    file.println("Device type: " + printableUID.picc_type);
    file.println("# UID, ATQA and SAK are common for all formats");
    file.println("UID: " + printableUID.uid);
    if (printableUID.picc_type != "FeliCa") {
        file.println("SAK: " + printableUID.sak);
        file.println("ATQA: " + printableUID.atqa);
        file.println("# Memory dump");
        file.println("Pages total: " + String(dataPages));
        if (!pageReadSuccess) file.println("Pages read: " + String(dataPages));
    } else {
        file.println("Manufacture id: " + printableUID.sak);
        file.println("Blocks total: " + String(totalPages));
        file.println("Blocks read: " + String(dataPages));
    }
    file.print(strAllPages);

    file.close();
    delay(100);
    return SUCCESS;
}

String PN532::get_tag_type() {
    String tag_type = "Unknown"; // nfc.PICC_GetTypeName(targetUid.sak);
    if (targetUid.sak == 0x08) tag_type = "MIFARE Classic 1K";
    else if (targetUid.sak == 0x18) tag_type = "MIFARE Classic 4K";
    else if (targetUid.sak == 0x00) tag_type = "MIFARE Ultralight";
    else if (targetUid.sak == 0x09) tag_type = "MIFARE Mini";

    if (targetUid.sak == PICC_TYPE_MIFARE_UL) {
        switch (totalPages) {
            case 45: tag_type = "NTAG213"; break;
            case 135: tag_type = "NTAG215"; break;
            case 231: tag_type = "NTAG216"; break;
            default: break;
        }
    }

    return tag_type;
}

void PN532::set_uid() {
    uid.sak = targetUid.sak;
    uid.size = targetUid.size;

    for (byte i = 0; i < 2; i++) uid.atqaByte[i] = targetUid.atqaByte[i];

    for (byte i = 0; i < targetUid.size; i++) { uid.uidByte[i] = targetUid.uidByte[i]; }
}

void PN532::format_data() {
    byte bcc = 0;

    printableUID.picc_type = get_tag_type();

    printableUID.sak = targetUid.sak < 0x10 ? "0" : "";
    printableUID.sak += String(targetUid.sak, HEX);
    printableUID.sak.toUpperCase();

    // UID
    for (byte i = 0; i < targetUid.size; i++) { bcc = bcc ^ targetUid.uidByte[i]; }
    printableUID.uid = hexToStr(targetUid.uidByte, targetUid.size);

    // BCC
    printableUID.bcc = bcc < 0x10 ? "0" : "";
    printableUID.bcc += String(bcc, HEX);
    printableUID.bcc.toUpperCase();

    // ATQA
    printableUID.atqa = hexToStr(targetUid.atqaByte, 2);
}

void PN532::format_data_felica(uint8_t idm[8], uint8_t pmm[8], uint16_t sys_code) {
    // Reuse uid-sak-atqa to save memory
    printableUID.picc_type = "FeliCa";
    printableUID.uid = hexToStr(idm, 8);
    printableUID.sak = hexToStr(pmm, 8);
    printableUID.atqa = String(sys_code, HEX);

    memcpy(uid.uidByte, idm, 8);
}

void PN532::parse_data() {
    String strUID = printableUID.uid;
    strUID.trim();
    strUID.replace(" ", "");
    uid.size = strUID.length() / 2;
    for (size_t i = 0; i < strUID.length(); i += 2) {
        uid.uidByte[i / 2] = strtoul(strUID.substring(i, i + 2).c_str(), NULL, 16);
    }

    printableUID.sak.trim();
    uid.sak = strtoul(printableUID.sak.c_str(), NULL, 16);

    String strAtqa = printableUID.atqa;
    strAtqa.trim();
    strAtqa.replace(" ", "");
    for (size_t i = 0; i < strAtqa.length(); i += 2) {
        uid.atqaByte[i / 2] = strtoul(strAtqa.substring(i, i + 2).c_str(), NULL, 16);
    }
}

int PN532::read_data_blocks() {
    dataPages = 0;
    totalPages = 0;
    int readStatus = FAILURE;

    strAllPages = "";

    if (printableUID.picc_type != "FeliCa") {
        switch (uid.sak) {
            case PICC_TYPE_MIFARE_MINI:
            case PICC_TYPE_MIFARE_1K:
            case PICC_TYPE_MIFARE_4K: readStatus = read_mifare_classic_data_blocks(); break;

            case PICC_TYPE_MIFARE_UL:
                readStatus = read_mifare_ultralight_data_blocks();
                if (totalPages == 0) totalPages = dataPages;
                break;

            default: break;
        }
    } else {
        readStatus = read_felica_data();
    }

    return readStatus;
}

int PN532::read_mifare_classic_data_blocks() {
    byte no_of_sectors = 0;
    int sectorReadStatus = FAILURE;

    switch (uid.sak) {
        case PICC_TYPE_MIFARE_MINI:
            no_of_sectors = 5;
            totalPages = 20; // 320 bytes / 16 bytes per page
            break;

        case PICC_TYPE_MIFARE_1K:
            no_of_sectors = 16;
            totalPages = 64; // 1024 bytes / 16 bytes per page
            break;

        case PICC_TYPE_MIFARE_4K:
            no_of_sectors = 40;
            totalPages = 256; // 4096 bytes / 16 bytes per page
            break;

        default: // Should not happen. Ignore.
            break;
    }

    if (no_of_sectors) {
        for (int8_t i = 0; i < no_of_sectors; i++) {
            sectorReadStatus = read_mifare_classic_data_sector(i);
            if (sectorReadStatus != SUCCESS) break;
        }
    }
    return sectorReadStatus;
}

int PN532::read_mifare_classic_data_sector(byte sector) {
    byte firstBlock;
    byte no_of_blocks;

    if (sector < 32) {
        no_of_blocks = 4;
        firstBlock = sector * no_of_blocks;
    } else if (sector < 40) {
        no_of_blocks = 16;
        firstBlock = 128 + (sector - 32) * no_of_blocks;
    } else {
        return FAILURE;
    }

    byte buffer[18];
    byte blockAddr;
    String strPage;

    int authStatus = authenticate_mifare_classic(firstBlock);
    if (authStatus != SUCCESS) return authStatus;

    for (int8_t blockOffset = 0; blockOffset < no_of_blocks; blockOffset++) {
        strPage = "";
        blockAddr = firstBlock + blockOffset;

        if (!nfc.mifareclassic_ReadDataBlock(blockAddr, buffer)) return FAILURE;

        strPage = hexToStr(buffer, 16);

        strAllPages += "Page " + String(dataPages) + ": " + strPage + "\n";
        dataPages++;
    }

    return SUCCESS;
}

int PN532::authenticate_mifare_classic(byte block) {
    uint8_t successA = 0;
    uint8_t successB = 0;

    for (auto key : keys) {
        successA = nfc.mifareclassic_AuthenticateBlock(uid.uidByte, uid.size, block, 0, key);
        if (successA) break;

        if (!nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A) || !readDetectedTarget()) {
            return TAG_NOT_PRESENT;
        }
    }

    if (!successA) {
        uint8_t keyA[6];

        for (const auto &mifKey : leleConfig.mifareKeys) {
            for (size_t i = 0; i < mifKey.length(); i += 2) {
                keyA[i / 2] = strtoul(mifKey.substring(i, i + 2).c_str(), NULL, 16);
            }

            successA = nfc.mifareclassic_AuthenticateBlock(uid.uidByte, uid.size, block, 0, keyA);
            if (successA) break;

            if (!nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A) || !readDetectedTarget()) {
                return TAG_NOT_PRESENT;
            }
        }
    }

    for (auto key : keys) {
        successB = nfc.mifareclassic_AuthenticateBlock(uid.uidByte, uid.size, block, 1, key);
        if (successB) break;

        if (!nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A) || !readDetectedTarget()) {
            return TAG_NOT_PRESENT;
        }
    }

    if (!successB) {
        uint8_t keyB[6];

        for (const auto &mifKey : leleConfig.mifareKeys) {
            for (size_t i = 0; i < mifKey.length(); i += 2) {
                keyB[i / 2] = strtoul(mifKey.substring(i, i + 2).c_str(), NULL, 16);
            }

            successB = nfc.mifareclassic_AuthenticateBlock(uid.uidByte, uid.size, block, 1, keyB);
            if (successB) break;

            if (!nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A) || !readDetectedTarget()) {
                return TAG_NOT_PRESENT;
            }
        }
    }

    return (successA && successB) ? SUCCESS : TAG_AUTH_ERROR;
}

int PN532::read_mifare_ultralight_data_blocks() {
    uint8_t success;
    byte buffer[18];
    byte i;
    String strPage = "";

    uint8_t buf[4];
    nfc.mifareultralight_ReadPage(3, buf);
    switch (buf[2]) {
        // NTAG213
        case 0x12: totalPages = 45; break;
        // NTAG215
        case 0x3E: totalPages = 135; break;
        // NTAG216
        case 0x6D: totalPages = 231; break;
        // MIFARE UL
        default: totalPages = 64; break;
    }

    for (byte page = 0; page < totalPages; page += 4) {
        success = nfc.ntag2xx_ReadPage(page, buffer);
        if (!success) return FAILURE;

        for (byte offset = 0; offset < 4; offset++) {
            strPage = "";
            for (byte index = 0; index < 4; index++) {
                i = 4 * offset + index;
                strPage += buffer[i] < 0x10 ? F(" 0") : F(" ");
                strPage += String(buffer[i], HEX);
            }
            strPage.trim();
            strPage.toUpperCase();

            strAllPages += "Page " + String(dataPages) + ": " + strPage + "\n";
            dataPages++;
            if (dataPages >= totalPages) break;
        }
    }

    return SUCCESS;
}

int PN532::read_felica_data() {
    // Felica disabled
    return SUCCESS;
}

int PN532::write_data_blocks() {
    String pageLine = "";
    String strBytes = "";
    int lineBreakIndex;
    int pageIndex;
    bool blockWriteSuccess;
    int totalSize = strAllPages.length();

    Serial.println(strAllPages);
    while (strAllPages.length() > 0) {
        lineBreakIndex = strAllPages.indexOf("\n");
        pageLine = strAllPages.substring(0, lineBreakIndex);
        strAllPages = strAllPages.substring(lineBreakIndex + 1);

        pageIndex = pageLine.substring(5, pageLine.indexOf(":")).toInt();
        strBytes = pageLine.substring(pageLine.indexOf(":") + 1);
        strBytes.trim();

        if (pageIndex == 0) continue;

        if (printableUID.picc_type != "FeliCa") {
            switch (uid.sak) {
                case PICC_TYPE_MIFARE_MINI:
                case PICC_TYPE_MIFARE_1K:
                case PICC_TYPE_MIFARE_4K:
                    if (pageIndex == 0 || (pageIndex + 1) % 4 == 0)
                        continue; // Data blocks for MIFARE Classic
                    blockWriteSuccess = write_mifare_classic_data_block(pageIndex, strBytes);
                    break;

                case PICC_TYPE_MIFARE_UL:
                    if (pageIndex < 4 || pageIndex >= dataPages - 5) continue; // Data blocks for NTAG21X
                    blockWriteSuccess = write_mifare_ultralight_data_block(pageIndex, strBytes);
                    break;

                default: blockWriteSuccess = false; break;
            }
        } else {
            blockWriteSuccess = write_felica_data_block(pageIndex, strBytes);
        }

        if (!blockWriteSuccess) return FAILURE;

        progressHandler(totalSize - strAllPages.length(), totalSize, "Writing data blocks...");
    }

    return SUCCESS;
}

bool PN532::write_mifare_classic_data_block(int block, String data) {
    data.replace(" ", "");
    byte size = data.length() / 2;
    byte buffer[size];

    if (size != 16) return false;

    for (size_t i = 0; i < data.length(); i += 2) {
        buffer[i / 2] = strtoul(data.substring(i, i + 2).c_str(), NULL, 16);
    }

    if (authenticate_mifare_classic(block) != SUCCESS) return false;

    return nfc.mifareclassic_WriteDataBlock(block, buffer);
}

bool PN532::write_mifare_ultralight_data_block(int block, String data) {
    data.replace(" ", "");
    byte size = data.length() / 2;
    byte buffer[size];

    if (size != 4) return false;

    for (size_t i = 0; i < data.length(); i += 2) {
        buffer[i / 2] = strtoul(data.substring(i, i + 2).c_str(), NULL, 16);
    }

    return nfc.ntag2xx_WritePage(block, buffer);
}

int PN532::write_felica_data_block(int block, String data) {
    return false; // Felica disabled
}

int PN532::erase_data_blocks() {
    bool blockWriteSuccess;

    switch (uid.sak) {
        case PICC_TYPE_MIFARE_MINI:
        case PICC_TYPE_MIFARE_1K:
        case PICC_TYPE_MIFARE_4K:
            for (byte i = 1; i < 64; i++) {
                if ((i + 1) % 4 == 0) continue;
                blockWriteSuccess =
                    write_mifare_classic_data_block(i, "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00");
                if (!blockWriteSuccess) return FAILURE;
            }
            break;

        case PICC_TYPE_MIFARE_UL:
            // NDEF stardard
            blockWriteSuccess = write_mifare_ultralight_data_block(4, "03 00 FE 00");
            if (!blockWriteSuccess) return FAILURE;

            for (byte i = 5; i < 130; i++) {
                blockWriteSuccess = write_mifare_ultralight_data_block(i, "00 00 00 00");
                if (!blockWriteSuccess) return FAILURE;
            }
            break;

        default: break;
    }

    return SUCCESS;
}

int PN532::write_ndef_blocks() {
    if (uid.sak != PICC_TYPE_MIFARE_UL) return TAG_NOT_MATCH;

    byte ndef_size = ndefMessage.messageSize + 3;
    byte payload_size = ndef_size % 4 == 0 ? ndef_size : ndef_size + (4 - (ndef_size % 4));
    byte ndef_payload[payload_size];
    byte i;
    bool blockWriteSuccess;
    uint8_t success;

    ndef_payload[0] = ndefMessage.begin;
    ndef_payload[1] = ndefMessage.messageSize;
    ndef_payload[2] = ndefMessage.header;
    ndef_payload[3] = ndefMessage.tnf;
    ndef_payload[4] = ndefMessage.payloadSize;
    ndef_payload[5] = ndefMessage.payloadType;

    for (i = 0; i < ndefMessage.payloadSize; i++) { ndef_payload[i + 6] = ndefMessage.payload[i]; }

    ndef_payload[ndef_size - 1] = ndefMessage.end;

    if (payload_size > ndef_size) {
        for (i = ndef_size; i < payload_size; i++) { ndef_payload[i] = 0; }
    }

    for (int i = 0; i < payload_size; i += 4) {
        int block = 4 + (i / 4);
        success = nfc.ntag2xx_WritePage(block, ndef_payload + i);
        if (!success) return FAILURE;
    }

    return SUCCESS;
}
