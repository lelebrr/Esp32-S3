#include "sd_adapter.h"

SPIClass *SDAdapter::spi = NULL;

bool SDAdapter::init() {
    if (spi != NULL) {
        return true; 
    }

    // Use HSPI for SD Card (dedicated bus, avoids TFT conflicts)
    spi = new SPIClass(HSPI);
    // Init SPI: SCK, MISO, MOSI, CS
    spi->begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
    
    // Mount SD Card at 40MHz
    if (!SD.begin(PIN_SD_CS, *spi, 40000000)) {
        Serial.println("Card Mount Failed");
        return false;
    }

    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return false;
    }

    Serial.println("SD Card Initialized Successfully");
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    
    return true;
}

void SDAdapter::listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels) {
                listDir(fs, file.path(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void SDAdapter::readFile(fs::FS &fs, const char * path) {
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}

void SDAdapter::writeFile(fs::FS &fs, const char * path, const char * message) {
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(message)) {
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

bool SDAdapter::appendFile(fs::FS &fs, const char * path, const char * message) {
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending");
        return false;
    }
    bool res = false;
    if (file.print(message)) {
        Serial.println("Message appended");
        res = true;
    } else {
        Serial.println("Append failed");
    }
    file.close();
    return res;
}

bool SDAdapter::mkdir(fs::FS &fs, const char * path) {
    Serial.printf("Creating Dir: %s\n", path);
    if (fs.mkdir(path)) {
        Serial.println("Dir created");
        return true;
    } else {
        Serial.println("mkdir failed");
        return false;
    }
}

bool SDAdapter::rmdir(fs::FS &fs, const char * path) {
    Serial.printf("Removing Dir: %s\n", path);
    if (fs.rmdir(path)) {
        Serial.println("Dir removed");
        return true;
    } else {
        Serial.println("rmdir failed");
        return false;
    }
}

bool SDAdapter::exists(fs::FS &fs, const char * path) {
    return fs.exists(path);
}
