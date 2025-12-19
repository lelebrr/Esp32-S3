#include "IRMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/utils.h"
#include "modules/ir/TV-B-Gone.h"
#include "modules/ir/custom_ir.h"
#include "modules/ir/ir_bruteforce.h"
#include "modules/ir/ir_jammer.h"
#include "modules/ir/ir_read.h"
#include "modules/ir/tv_nuke.h"

void IRMenu::optionsMenu() {
    options = {
        {"TV-B-Gone",        StartTvBGone              },
        {"AC Bruteforce",    ir_bruteforce_ac          }, // NOVO
        {"Univ. Vol Mute",   ir_bruteforce_volume      }, // NOVO
        {"TV NUKE",
         [=]() {
             tv_nuke_init();
             tv_nuke_start();
             while (tv_nuke_is_active()) {
                 tv_nuke_update();
                 if (check(AnyKeyPress)) {
                     tv_nuke_stop();
                     break;
                 }
             }
         }                                             },
        {"IR Personalizado", otherIRcodes              },
        {"IR Read",          [=]() { IrRead(); }       },
#if !defined(LITE_VERSION)
        {"IR Jammer",        startIrJammer             }, // Simple frequency-adjustable jammer
#endif
        {"Configurar",       [this]() { configMenu(); }},
    };
    addOptionToMainMenu();

    String txt = "Infrared";
    txt += " Tx: " + String(leleConfig.irTx) + " Rx: " + String(leleConfig.irRx) +
           " Rpts: " + String(leleConfig.irTxRepeats);
    loopOptions(options, MENU_TYPE_SUBMENU, txt.c_str());
}

void IRMenu::configMenu() {
    options = {
        {"Ir TX Pin", lambdaHelper(gsetIrTxPin, true)},
        {"Ir RX Pin", lambdaHelper(gsetIrRxPin, true)},
        {"Ir TX Repeats", setIrTxRepeats},
        {"Voltar", [this]() { optionsMenu(); }},
    };

    loopOptions(options, MENU_TYPE_SUBMENU, "IR Config");
}
void IRMenu::drawIconImg() {
    drawImg(
        *leleConfig.themeFS(), leleConfig.getThemeItemImg(leleConfig.theme.paths.ir), 0, imgCenterY, true
    );
}
void IRMenu::drawIcon(float scale) {
    clearIconArea();
    int iconSize = scale * 60;
    int radius = scale * 7;
    int deltaRadius = scale * 10;

    if (iconSize % 2 != 0) iconSize++;

    tft.fillRect(
        iconCenterX - iconSize / 2, iconCenterY - iconSize / 2, iconSize / 6, iconSize, leleConfig.priColor
    );
    tft.fillRect(
        iconCenterX - iconSize / 3,
        iconCenterY - iconSize / 3,
        iconSize / 6,
        2 * iconSize / 3,
        leleConfig.priColor
    );

    tft.drawCircle(iconCenterX - iconSize / 6, iconCenterY, radius, leleConfig.priColor);

    tft.drawArc(
        iconCenterX - iconSize / 6,
        iconCenterY,
        2.5 * radius,
        2 * radius,
        220,
        320,
        leleConfig.priColor,
        leleConfig.bgColor
    );
    tft.drawArc(
        iconCenterX - iconSize / 6,
        iconCenterY,
        2.5 * radius + deltaRadius,
        2 * radius + deltaRadius,
        220,
        320,
        leleConfig.priColor,
        leleConfig.bgColor
    );
    tft.drawArc(
        iconCenterX - iconSize / 6,
        iconCenterY,
        2.5 * radius + 2 * deltaRadius,
        2 * radius + 2 * deltaRadius,
        220,
        320,
        leleConfig.priColor,
        leleConfig.bgColor
    );
}
