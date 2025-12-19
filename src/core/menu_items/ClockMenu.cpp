#include "ClockMenu.h"
#include "core/display.h"
#include "core/settings.h"

void ClockMenu::optionsMenu() { runClockLoop(); }
void ClockMenu::drawIconImg() {
    drawImg(
        *leleConfig.themeFS(),
        leleConfig.getThemeItemImg(leleConfig.theme.paths.clock),
        0,
        imgCenterY,
        true
    );
}
void ClockMenu::drawIcon(float scale) {
    clearIconArea();
    int radius = scale * 30;
    int pointerSize = scale * 15;

    // Case
    tft.drawArc(
        iconCenterX, iconCenterY, 1.1 * radius, radius, 0, 360, leleConfig.priColor, leleConfig.bgColor
    );

    // Pivot center
    tft.fillCircle(iconCenterX, iconCenterY, radius / 10, leleConfig.priColor);

    // Hours & minutes
    tft.drawLine(
        iconCenterX,
        iconCenterY,
        iconCenterX - 2 * pointerSize / 3,
        iconCenterY - 2 * pointerSize / 3,
        leleConfig.priColor
    );
    tft.drawLine(
        iconCenterX, iconCenterY, iconCenterX + pointerSize, iconCenterY - pointerSize, leleConfig.priColor
    );
}
