#include "FileMenu.h"
#include "core/display.h"
#include "core/massStorage.h"
#include "core/sd_functions.h"
#include "core/utils.h"
#include "core/wifi/webInterface.h"

void FileMenu::optionsMenu() {
    options.clear();
    if (setupSdCard()) options.push_back({"Cart�o SD", [=]() { loopSD(SD); }});
    options.push_back({"LittleFS", [=]() { loopSD(LittleFS); }});
    options.push_back({"Interface Web", loopOptionsWebUi});

#if defined(SOC_USB_OTG_SUPPORTED) && !defined(USE_SD_MMC)
    options.push_back({"Armazenamento USB", [=]() { MassStorage(); }});
#endif
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "Arquivos");
}
void FileMenu::drawIconImg() {
    drawImg(
        *leleConfig.themeFS(),
        leleConfig.getThemeItemImg(leleConfig.theme.paths.files),
        0,
        imgCenterY,
        true
    );
}
void FileMenu::drawIcon(float scale) {
    clearIconArea();
    int iconW = scale * 32;
    int iconH = scale * 48;

    if (iconW % 2 != 0) iconW++;
    if (iconH % 2 != 0) iconH++;

    int foldSize = iconH / 4;
    int iconX = iconCenterX - iconW / 2;
    int iconY = iconCenterY - iconH / 2;
    int iconDelta = 10;

    // Files
    tft.drawRect(iconX + iconDelta, iconY - iconDelta, iconW, iconH, leleConfig.priColor);

    tft.fillRect(iconX, iconY, iconW, iconH, leleConfig.bgColor);
    tft.drawRect(iconX, iconY, iconW, iconH, leleConfig.priColor);

    tft.fillRect(iconX - iconDelta, iconY + iconDelta, iconW, iconH, leleConfig.bgColor);
    tft.drawRect(iconX - iconDelta, iconY + iconDelta, iconW, iconH, leleConfig.priColor);

    // Erase corners
    tft.fillRect(
        iconX + iconDelta + iconW - foldSize, iconY - iconDelta - 1, foldSize, 2, leleConfig.bgColor
    );
    tft.fillRect(iconX + iconDelta + iconW - 1, iconY - iconDelta, 2, foldSize, leleConfig.bgColor);

    tft.fillRect(iconX + iconW - foldSize, iconY - 1, foldSize, 2, leleConfig.bgColor);
    tft.fillRect(iconX + iconW - 1, iconY, 2, foldSize, leleConfig.bgColor);

    tft.fillRect(
        iconX - iconDelta + iconW - foldSize, iconY + iconDelta - 1, foldSize, 2, leleConfig.bgColor
    );
    tft.fillRect(iconX - iconDelta + iconW - 1, iconY + iconDelta, 2, foldSize, leleConfig.bgColor);

    // Folds
    tft.drawTriangle(
        iconX + iconDelta + iconW - foldSize,
        iconY - iconDelta,
        iconX + iconDelta + iconW - foldSize,
        iconY - iconDelta + foldSize - 1,
        iconX + iconDelta + iconW - 1,
        iconY - iconDelta + foldSize - 1,
        leleConfig.priColor
    );
    tft.drawTriangle(
        iconX + iconW - foldSize,
        iconY,
        iconX + iconW - foldSize,
        iconY + foldSize - 1,
        iconX + iconW - 1,
        iconY + foldSize - 1,
        leleConfig.priColor
    );
    tft.drawTriangle(
        iconX - iconDelta + iconW - foldSize,
        iconY + iconDelta,
        iconX - iconDelta + iconW - foldSize,
        iconY + iconDelta + foldSize - 1,
        iconX - iconDelta + iconW - 1,
        iconY + iconDelta + foldSize - 1,
        leleConfig.priColor
    );
}
