#ifndef SD_ADAPTER_H
#define SD_ADAPTER_H

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include "pin_config.h"

class SDAdapter {
public:
    static bool init();
    static void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
    static void readFile(fs::FS &fs, const char * path);
    static void writeFile(fs::FS &fs, const char * path, const char * message);
    static bool appendFile(fs::FS &fs, const char * path, const char * message);
    static bool mkdir(fs::FS &fs, const char * path);
    static bool rmdir(fs::FS &fs, const char * path);
    static bool exists(fs::FS &fs, const char * path);

private:
    static SPIClass *spi;
};

#endif // SD_ADAPTER_H
