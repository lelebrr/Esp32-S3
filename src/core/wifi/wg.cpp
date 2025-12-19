bool isConnectedWireguard = false;
#ifndef LITE_VERSION
#include "wg.h"
#include "core/display.h"
#include "core/sd_functions.h"
#include "core/wifi/wifi_common.h"
#include <WireGuard-ESP32.h>
#include <debug_log.h>
#include <globals.h>

char private_key[45];
IPAddress local_ip;
char public_key[45];
char endpoint_address[16];
int endpoint_port = 31337;

static constexpr const uint32_t UPDATE_INTERVAL_MS = 5000;

static WireGuard wg;

/*********************************************************************
**  Function: parse_config_file
**  parses wireguard config file wg.conf
**********************************************************************/
void parse_config_file(File configFile) {
    String line;

    while (configFile.available()) {
        line = configFile.readStringUntil('\n');
        // LOG_WIFI("==========PRINTING LINE");
        // LOG_WIFI("%s", line.c_str());
        line.trim();

        if (line.startsWith("[Interface]") || line.isEmpty()) {
            // Skip [Interface] or empty lines
            continue;
        } else if (line.startsWith("PrivateKey")) {
            line.remove(0, line.indexOf('=') + 1);
            line.trim();
            // LOG_WIFI("Private Key: %s", line.c_str());
            strncpy(private_key, line.c_str(), sizeof(private_key) - 1);
            private_key[sizeof(private_key) - 1] = '\0'; // Ensure null-terminated
        } else if (line.startsWith("Address")) {
            line.remove(0, line.indexOf('=') + 1);
            line.trim();
            LOG_WIFI("Local IP: %s", line.c_str());
            int slashIndex = line.indexOf('/');

            if (slashIndex != -1) {
                // LOG_WIFI("~~~~~~~~~~~~");
                // LOG_WIFI("%s", line.substring(0, slashIndex).c_str());
                local_ip.fromString(line.substring(0, slashIndex));
            }

        } else if (line.startsWith("[Peer]")) {
            // add [Peer] section
        } else if (line.startsWith("PublicKey")) {
            line.remove(0, line.indexOf('=') + 1);
            line.trim();
            LOG_WIFI("Public Key: %s", line.c_str());
            strncpy(public_key, line.c_str(), sizeof(public_key) - 1);
            public_key[sizeof(public_key) - 1] = '\0'; // Ensure null-terminated
        } else if (line.startsWith("Endpoint")) {
            line.remove(0, line.indexOf('=') + 1);
            line.trim();
            int colonIndex = line.indexOf(':');

            if (colonIndex != -1) {
                // Serial.println("Endpoint Line: " + line);
                strncpy(
                    endpoint_address, line.substring(0, colonIndex).c_str(), sizeof(endpoint_address) - 1
                );
                endpoint_address[sizeof(endpoint_address) - 1] = '\0'; // Ensure null-terminated
                LOG_WIFI("Endpoint Address: %s", endpoint_address);
                endpoint_port = line.substring(colonIndex + 1).toInt();
                LOG_WIFI("Endpoint Port: %d", endpoint_port);
            }
        }
    }
    LOG_WIFI("Closing file!");
    configFile.close();
}

/*********************************************************************
**  Function: read_and_parse_file
**  tries to open file wg.conf on local SD
**********************************************************************/
void read_and_parse_file() {
    if (!setupSdCard()) {
        sdcardMounted = false;
        LOG_ERROR("Failed to initialize SD card");
        return;
    }

    File file = SD.open("/wg.conf");
    if (!file) {
        tft.fillScreen(leleConfig.bgColor);
        tft.setCursor(0, 0);

        tft.setTextColor(TFT_RED, leleConfig.bgColor);
        LOG_ERROR("Failed to open wg.conf file");
        // tft.println("No wg.conf file\nfound on\nthe SD");
        displayRedStripe("No wg.conf file", TFT_RED, leleConfig.priColor);
        tft.setTextColor(leleConfig.priColor, leleConfig.bgColor);
        delay(6000);
        return;
    }

    LOG_WIFI("Read config file!");

    LOG_WIFI("Found file!");
    parse_config_file(file);
}

/*********************************************************************
**  Function: wg_setup
**  connect to wireguard tunnel
**********************************************************************/
void wg_setup() {
    if (!wifiConnected) wifiConnectMenu();

    read_and_parse_file();

    LOG_WIFI("Adjusting system time...");
    configTime(9 * 60 * 60, 0, "ntp.jst.mfeed.ad.jp", "ntp.nict.jp");
    tft.fillScreen(leleConfig.bgColor);
    tft.setCursor(0, 0);

    LOG_WIFI("Connected. Initializing WireGuard...");
    // tft.println("Connecting to\nwireguard...");
    wg.begin(local_ip, private_key, endpoint_address, public_key, endpoint_port);
    LOG_WIFI("%s", local_ip.toString().c_str());
    // Serial.println(private_key); // Security risk
    LOG_WIFI("%s", endpoint_address);
    LOG_WIFI("%s", public_key);
    LOG_WIFI("%d", endpoint_port);

    tft.fillScreen(leleConfig.bgColor);
    tft.setCursor(0, 0);
    tft.setTextSize(FG);

    tft.setTextColor(TFT_GREEN, leleConfig.bgColor);
    tft.println("Connected!");
    tft.setTextColor(leleConfig.priColor, leleConfig.bgColor);
    tft.println("IP on tunnel:");
    tft.setTextColor(TFT_WHITE, leleConfig.bgColor);
    tft.println(local_ip);
    tft.setTextColor(leleConfig.priColor, leleConfig.bgColor);
    LOG_WIFI("%s", local_ip.toString().c_str());
    delay(7000);
    isConnectedWireguard = true;
    tft.fillScreen(leleConfig.bgColor);
}
#endif
