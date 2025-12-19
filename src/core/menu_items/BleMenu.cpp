/**
 * @file BleMenu.cpp
 * @brief Menu de funcionalidades Bluetooth LE
 * @author Lele Origin Team
 *
 * Ataques BLE funcionais:
 * - Applejuice (iOS popup spam)
 * - SourApple (iOS crash)
 * - Spam Windows Swift Pair
 * - Spam Samsung
 * - Spam Android
 * - Spam Personalizado
 */

#include "BleMenu.h"
#include "core/display.h"
#include "core/utils.h"
#include "modules/badusb_ble/ducky_typer.h"
#include "modules/ble/ble_common.h"
#include "modules/ble/ble_ninebot.h"
#include "modules/ble/ble_spam.h"
#include "modules/ble/ble_continuity_spoof.h"
#include <globals.h>

void BleMenu::optionsMenu() {
    options.clear();
    if (BLEConnected) {
        options.push_back({"Desconectar", [=]() {
                               BLEDevice::deinit();
                               BLEConnected = false;
                               delete hid_ble;
                               hid_ble = nullptr;
                               if (_Ask_for_restart == 1)
                                   _Ask_for_restart = 2;
                           }});
    }

    options.push_back({"Comandos Midia", [=]() { MediaCommands(hid_ble, true); }});
#if !defined(LITE_VERSION)
    options.push_back({"Escanear BLE", ble_scan});
    options.push_back({"iBeacon", [=]() { ibeacon(); }});
    options.push_back({"BLE Malicioso", [=]() { ducky_setup(hid_ble, true); }});
#endif
    options.push_back({"Teclado BLE", [=]() { ducky_keyboard(hid_ble, true); }});
    options.push_back({"BLE Keyboard Inject", [=]() { ble_keyboard_inject_attack(); }});

    // Submenu for Pentest Attacks
    options.push_back({"Pentest Attacks", [this]() {
        std::vector<Option> attackOptions;
        attackOptions.push_back({"Continuity Spoof", [=]() { runContinuitySpoof(); }});
        attackOptions.push_back({"LowbattBLE Exploit", [=]() { runLowbattBLE(); }});
        attackOptions.push_back({"AirTag Spoof", [=]() { runAirTagSpoof(); }});
        attackOptions.push_back({"Audio Bleed", [=]() { runAudioBleed(); }});
        attackOptions.push_back({"BLE Rootkit Injection", [=]() { runBLENetworkDriverSpoof(); }});
        attackOptions.push_back({"--- APP ATTACKS ---", [=]() {}});
        attackOptions.push_back({"Uber BLE Attack", [=]() { runUberBLE(); }});
        attackOptions.push_back({"Netflix BLE Attack", [=]() { runNetflixBLE(); }});
        attackOptions.push_back({"iFood BLE Attack", [=]() { runiFoodBLE(); }});
        attackOptions.push_back({"Spotify BLE Attack", [=]() { runSpotifyBLE(); }});
        attackOptions.push_back({"Instagram BLE Attack", [=]() { runInstagramBLE(); }});
        attackOptions.push_back({"BLE Deauth Attack", [=]() { runBLEDeauthAttack(); }});
        attackOptions.push_back({"Voltar", [this]() { optionsMenu(); }});
        loopOptions(attackOptions, MENU_TYPE_SUBMENU, "Pentest Attacks");
    }});

    // Submenu BLE Spam (agrupado)
    options.push_back({"BLE Spam", [this]() {
        std::vector<Option> spamOptions;
        spamOptions.push_back({"Applejuice", lambdaHelper(aj_adv, 0)});
        spamOptions.push_back({"SourApple", lambdaHelper(aj_adv, 1)});
        spamOptions.push_back({"Spam Windows", lambdaHelper(aj_adv, 2)});
        spamOptions.push_back({"Spam Samsung", lambdaHelper(aj_adv, 3)});
        spamOptions.push_back({"Spam Android", lambdaHelper(aj_adv, 4)});
        spamOptions.push_back({"Spam Todos", lambdaHelper(aj_adv, 5)});
        spamOptions.push_back({"Personalizado", lambdaHelper(aj_adv, 6)});
        spamOptions.push_back({"Voltar", [this]() { optionsMenu(); }});
        loopOptions(spamOptions, MENU_TYPE_SUBMENU, "BLE Spam");
    }});

#if !defined(LITE_VERSION)
    options.push_back({"Ninebot", [=]() { BLENinebot(); }});
#endif
    addOptionToMainMenu();
    loopOptions(options, MENU_TYPE_SUBMENU, "Bluetooth");
}

void BleMenu::drawIconImg() {
    drawImg(
        *leleConfig.themeFS(), leleConfig.getThemeItemImg(leleConfig.theme.paths.ble), 0, imgCenterY, true
    );
}

void BleMenu::drawIcon(float scale) {
    clearIconArea();
    int lineWidth = scale * 5;
    int iconW = scale * 36;
    int iconH = scale * 60;
    int radius = scale * 5;
    int deltaRadius = scale * 10;
    if (iconW % 2 != 0) iconW++;
    if (iconH % 4 != 0) iconH += 4 - (iconH % 4);
    tft.drawWideLine(iconCenterX, iconCenterY + iconH / 4, iconCenterX - iconW, iconCenterY - iconH / 4, lineWidth, leleConfig.priColor, leleConfig.priColor);
    tft.drawWideLine(iconCenterX, iconCenterY - iconH / 4, iconCenterX - iconW, iconCenterY + iconH / 4, lineWidth, leleConfig.priColor, leleConfig.priColor);
    tft.drawWideLine(iconCenterX, iconCenterY + iconH / 4, iconCenterX - iconW / 2, iconCenterY + iconH / 2, lineWidth, leleConfig.priColor, leleConfig.priColor);
    tft.drawWideLine(iconCenterX, iconCenterY - iconH / 4, iconCenterX - iconW / 2, iconCenterY - iconH / 2, lineWidth, leleConfig.priColor, leleConfig.priColor);
    tft.drawWideLine(iconCenterX - iconW / 2, iconCenterY - iconH / 2, iconCenterX - iconW / 2, iconCenterY + iconH / 2, lineWidth, leleConfig.priColor, leleConfig.priColor);
    tft.drawArc(iconCenterX, iconCenterY, 2.5 * radius, 2 * radius, 210, 330, leleConfig.priColor, leleConfig.bgColor);
    tft.drawArc(iconCenterX, iconCenterY, 2.5 * radius + deltaRadius, 2 * radius + deltaRadius, 210, 330, leleConfig.priColor, leleConfig.bgColor);
    tft.drawArc(iconCenterX, iconCenterY, 2.5 * radius + 2 * deltaRadius, 2 * radius + 2 * deltaRadius, 210, 330, leleConfig.priColor, leleConfig.bgColor);
}
