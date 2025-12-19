
#include "ConnectMenu.h"
#include "core/connect/file_sharing.h"
#include "core/connect/serial_commands.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/utils.h"
#include "core/wifi/wifi_common.h"

void ConnectMenu::optionsMenu() {
    options = {
#ifndef LITE_VERSION
        {"Enviar Arquivo", [=]() { FileSharing().sendFile(); }        },
        {"Recv File", [=]() { FileSharing().receiveFile(); }     },

        {"Send Cmds", [=]() { EspSerialCmd().sendCommands(); }   },
        {"Recv Cmds", [=]() { EspSerialCmd().receiveCommands(); }},
#endif
    };
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, getName().c_str());
}
void ConnectMenu::drawIconImg() {
    drawImg(
        *leleConfig.themeFS(),
        leleConfig.getThemeItemImg(leleConfig.theme.paths.connect),
        0,
        imgCenterY,
        true
    );
}
void ConnectMenu::drawIcon(float scale) {
    clearIconArea();

    int iconW = scale * 50;
    int iconH = scale * 40;
    int radius = scale * 7;

    if (iconW % 2 != 0) iconW++;
    if (iconH % 2 != 0) iconH++;

    tft.fillCircle(iconCenterX - iconW / 2, iconCenterY, radius, leleConfig.priColor);

    tft.fillCircle(iconCenterX + 0.3 * iconW, iconCenterY - iconH / 2, radius, leleConfig.priColor);
    tft.fillCircle(iconCenterX + 0.5 * iconW, iconCenterY, radius, leleConfig.priColor);
    tft.fillCircle(iconCenterX + 0.3 * iconW, iconCenterY + iconH / 2, radius, leleConfig.priColor);

    tft.drawLine(
        iconCenterX - iconW / 2,
        iconCenterY,
        iconCenterX + 0.3 * iconW,
        iconCenterY - iconH / 2,
        leleConfig.priColor
    );
    tft.drawLine(
        iconCenterX - iconW / 2, iconCenterY, iconCenterX + 0.5 * iconW, iconCenterY, leleConfig.priColor
    );
    tft.drawLine(
        iconCenterX - iconW / 2,
        iconCenterY,
        iconCenterX + 0.3 * iconW,
        iconCenterY + iconH / 2,
        leleConfig.priColor
    );
}
