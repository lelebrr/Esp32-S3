#include "OthersMenu.h"
#include "core/display.h"
#include "core/utils.h"
#include "modules/badusb_ble/ducky_typer.h"
#include "modules/bjs_interpreter/interpreter.h"
#include "modules/network_simulation.h"
#include "modules/others/clicker.h"
#include "modules/others/ibutton.h"
#include "modules/others/mic.h"
#include "modules/others/qrcode_menu.h"
#include "modules/others/timer.h"
#include "modules/others/tururururu.h"

void OthersMenu::optionsMenu() {
    options = {
        {"QRCodes",      qrcode_menu                              },
        {"Megalodon",    shark_setup                              },
#ifdef MIC_SPM1423
        {"Mic Spectrum", mic_test                                 },
        {"Mic Record",   mic_record                               }, //@deveclipse
#endif
#ifndef LITE_VERSION
        {"BadUSB",       [=]() { ducky_setup(hid_usb, false); }   },
        {"USB Keyboard", [=]() { ducky_keyboard(hid_usb, false); }},
#endif
#ifdef USB_as_HID
        {"Clicker",      clicker_setup                            },
#endif
#ifndef LITE_VERSION
        {"iButton",      setup_ibutton                            },
#endif
        {"Timer",        [=]() { Timer(); }                       },
        {"UART Keylogger", [=]() { uart_keylogger_attack(); }     },
        {"Network Sim",  [=]() {
            if (networkSim.begin()) {
                networkSim.start_simulation();
                tft.fillScreen(TFT_BLACK);
                tft.setTextColor(TFT_RED, TFT_BLACK);
                tft.drawCentreString("NETWORK SIMULATION ACTIVE", tftWidth/2, tftHeight/2, 2);
                tft.setTextSize(1);
                tft.drawCentreString("BLE: 1000pkt/s Core1 | WiFi: 1200pkt/s Core0", tftWidth/2, tftHeight/2 + 30, 1);
                tft.drawCentreString("Press any key to stop", tftWidth/2, tftHeight/2 + 50, 1);
                while (!check(AnyKeyPress)) {
                    delay(100);
                }
                networkSim.stop_simulation();
            } else {
                tft.fillScreen(TFT_BLACK);
                tft.setTextColor(TFT_RED, TFT_BLACK);
                tft.drawCentreString("SIMULATION INIT FAILED", tftWidth/2, tftHeight/2, 2);
                delay(2000);
            }
        } },
    };
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "Outros");
}
void OthersMenu::drawIconImg() {
    drawImg(
        *leleConfig.themeFS(),
        leleConfig.getThemeItemImg(leleConfig.theme.paths.others),
        0,
        imgCenterY,
        true
    );
}
void OthersMenu::drawIcon(float scale) {
    clearIconArea();
    int radius = scale * 7;

    tft.fillCircle(iconCenterX, iconCenterY, radius, leleConfig.priColor);

    tft.drawArc(
        iconCenterX, iconCenterY, 2.5 * radius, 2 * radius, 0, 340, leleConfig.priColor, leleConfig.bgColor
    );
    tft.drawArc(
        iconCenterX, iconCenterY, 3.5 * radius, 3 * radius, 20, 360, leleConfig.priColor, leleConfig.bgColor
    );
    tft.drawArc(
        iconCenterX, iconCenterY, 4.5 * radius, 4 * radius, 0, 200, leleConfig.priColor, leleConfig.bgColor
    );
    tft.drawArc(
        iconCenterX,
        iconCenterY,
        4.5 * radius,
        4 * radius,
        240,
        360,
        leleConfig.priColor,
        leleConfig.bgColor
    );
}
