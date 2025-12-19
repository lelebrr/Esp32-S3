#include "aggressive_sd.h"
#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <SD.h>
#include <SPI.h>
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include <sys/stat.h>
#include <unistd.h>

#define MOUNT_POINT "/sd"
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_NUM_CS   5

// Helper to recursively copy files/dirs from LittleFS to SD
static void recursive_copy_lfs_to_sd(File dir, String pathPrefix) {
    if (!dir) return;
    
    File file = dir.openNextFile();
    while(file) {
        String fileName = String(file.name());
        if(!fileName.startsWith("/")) fileName = "/" + fileName;
        
        // Full path in LittleFS (some implementations return relative, some full)
        // We assume valid name.
        
        String destPath = pathPrefix + fileName;
        
        if(file.isDirectory()) {
            Serial.printf("DIR: %s -> %s\n", fileName.c_str(), destPath.c_str());
            mkdir(destPath.c_str(), 0775);
            recursive_copy_lfs_to_sd(file, pathPrefix); // Recurse
        } else {
            Serial.printf("FILE: %s -> %s\n", fileName.c_str(), destPath.c_str());
            
            // Create parent directories if needed (simple check)
            // (Assumes directories are processed first or mkdir handles it)
            
            FILE* f_dst = fopen(destPath.c_str(), "wb");
            if(f_dst) {
                const size_t buf_size = 512;
                uint8_t buf[buf_size];
                while(file.available()) {
                    size_t n = file.read(buf, buf_size);
                    fwrite(buf, 1, n, f_dst);
                }
                fclose(f_dst);
            } else {
                Serial.printf("Err open dst: %s\n", destPath.c_str());
            }
        }
        file = dir.openNextFile();
    }
}

void aggressive_boot_logic() {
    Serial.println("Aggressive SD Boot Init...");

    // Mount SD via ESP-IDF VFS to ensure low-level access
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SPI3_HOST; // VSPI
    
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    
    // Initialize SPI
    esp_err_t ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        Serial.println("Failed to init SPI bus for Aggressive SD");
        return;
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = (gpio_num_t)PIN_NUM_CS;
    slot_config.host_id = host.slot;

    int retries = 0;
    while(true) {
        ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
        if (ret == ESP_OK) {
            Serial.println("SD up, flash limpo");
            break;
        } else {
             if (retries > 3) {
                 Serial.println("SD Mount Failed. Aborting Aggressive Boot.");
                 spi_bus_free(host.slot);
                 return;
             }
             Serial.println("Retrying SD Mount...");
             delay(1000);
             retries++;
        }
    }

    // SD Mounted. Mount LittleFS to read from it.
    if(LittleFS.begin(true)) {
         Serial.println("Backing up LittleFS to /sd/update...");
         
         // Ensure destination exists
         mkdir("/sd/update", 0775);
         
         File root = LittleFS.open("/");
         if (root) {
             // Start recursive copy to /sd/update
             recursive_copy_lfs_to_sd(root, "/sd/update");
             root.close();
         }
         
         // Wipe Flash
         Serial.println("Formatting LittleFS...");
         LittleFS.format();
         LittleFS.end();
    }
    
    // Cleanup
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    spi_bus_free(host.slot);
    Serial.println("Aggressive Boot Done.");
}
