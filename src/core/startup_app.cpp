/**
 * @file startup_app.cpp
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Lele startup apps
 * @version 0.1
 * @date 2024-11-20
 */

#include "startup_app.h"

#include "core/settings.h" // clock
#include "core/wifi/webInterface.h"
#include "core/wifi/wifi_common.h"
#include "modules/gps/gps_tracker.h"
#include "modules/gps/wardriving.h"
#include "modules/pwnagotchi/pwnagotchi.h"
#include "modules/rf/rf_send.h"
#include "modules/rfid/PN532KillerTools.h"
#include "modules/rfid/pn532ble.h"
#include "modules/wifi/sniffer.h"
#include <debug_log.h>
#ifdef SOC_USB_OTG_SUPPORTED
#include "core/massStorage.h"
#endif

StartupApp::StartupApp() {
#ifndef LITE_VERSION
    _startupApps["Lelegotchi"] = []() { Lelegotchi_start(); };
    _startupApps["Sniffer"] = []() { sniffer_setup(); };
#endif
    _startupApps["Clock"] = []() { runClockLoop(); };
    _startupApps["Custom SubGHz"] = []() { sendCustomRF(); };
    _startupApps["GPS Tracker"] = []() { GPSTracker(); };
#if defined(SOC_USB_OTG_SUPPORTED) && !defined(USE_SD_MMC)
    _startupApps["Mass Storage"] = []() { MassStorage(); };
#endif
    _startupApps["Wardriving"] = []() { Wardriving(); };
    _startupApps["WebUI"] = []() { startWebUi(!wifiConnecttoKnownNet()); };
#ifndef LITE_VERSION
    _startupApps["PN532 BLE"] = []() { Pn532ble(); };
    _startupApps["PN532 UART"] = []() { PN532KillerTools(); };
#endif
}

bool StartupApp::startApp(const String &appName) const {
    auto it = _startupApps.find(appName);
    if (it == _startupApps.end()) {
        LOG_DEBUG("Invalid startup app: %s", appName.c_str());
        return false;
    }

    it->second();

    delay(200);
    tft.fillScreen(leleConfig.bgColor);

    return true;
}

std::vector<String> StartupApp::getAppNames() const {
    std::vector<String> keys;
    for (const auto &pair : _startupApps) { keys.push_back(pair.first); }
    return keys;
}
