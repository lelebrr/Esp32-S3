#include "RFMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/utils.h"
#include "modules/rf/record.h"
#include "modules/rf/rf_bruteforce.h"
#include "modules/rf/rf_jammer.h"
#include "modules/rf/rf_listen.h"
#include "modules/rf/rf_presets.h"
#include "modules/rf/rf_scan.h"
#include "modules/rf/rf_send.h"
#include "modules/rf/rf_spectrum.h"
#include "modules/rf/rf_waterfall.h"
#include "modules/rf/rf_433_jammer.h"
#include "modules/rf/rf_ghost_replay.h"

void RFMenu::presetsMenu() {
    options = {
        {"--- VEICULOS ---",  [=]() {}                                        },
        {"Tesla Charge Port", [=]() { rf_run_preset(RF_PRESET_TESLA_CHARGE); }},

        {"--- PORTOES ---",   [=]() {}                                        },
        {"CAME 12-bit",       [=]() { rf_run_preset(RF_PRESET_CAME_12BIT); }  },
        {"CAME 24-bit",       [=]() { rf_run_preset(RF_PRESET_CAME_24BIT); }  },
        {"Nice Flor-S",       [=]() { rf_run_preset(RF_PRESET_NICE_FLOR); }   },

        {"--- AUTOMACAO ---", [=]() {}                                        },
        {"Ventilador Teto",   [=]() { rf_run_preset(RF_PRESET_FAN_CONTROL); } },

        {"--- EDUCATIVO ---", [=]() {}                                        },
        {"Rolling Code Sim",  [=]() { rf_run_rolling_code_sim(); }            },

        {"Voltar",            [this]() { optionsMenu(); }                     },
    };
    loopOptions(options, MENU_TYPE_SUBMENU, "RF Presets");
}

void RFMenu::optionsMenu() {
    options = {
        {"Escanear/Copiar",      [=]() { RFScan(); }        },
#if !defined(LITE_VERSION)
        {"Record RAW",           rf_raw_record              }, // Pablo-Ortiz-Lopez
        {"SubGhz Personalizado", sendCustomRF               },
#endif
        {"Espectro",             rf_spectrum                },
#if !defined(LITE_VERSION)
        {"SquareWave Spec",      rf_SquareWave              }, // @Pirata
        {"Spectogram",           rf_waterfall               }, // dev_eclipse
#if defined(BUZZ_PIN) or defined(HAS_NS4168_SPKR) and defined(RF_LISTEN_H)
        {"Listen",               rf_listen                  }, // dev_eclipse
#endif
        {"Bruteforce",           rf_bruteforce              }, // dev_eclipse
        {"Jammer Itmt",          [=]() { RFJammer(false); } },
#else
        {"Bruteforce", rf_bruteforce}, // dev_eclipse
#endif
        // --- EXPANSÃO GIGANTE DO MENU RF ---
        {"Simulacao & Atk",      [this]() { presetsMenu(); }},
        {"--- PENTEST ATTACKS ---", [=]() {}                },
        {"433MHz Jammer",        [=]() { rf_433_jammer(); } },
        {"Ghost Replay",         [=]() { rf_ghost_replay(); }},
        {"Jammer Total",         [=]() { RFJammer(true); }  },
        {"Configurar",           [this]() { configMenu(); } },
    };
    addOptionToMainMenu();

    delay(200);
    String txt = "Radio Frequency";
    if (leleConfig.rfModule == CC1101_SPI_MODULE) txt += " (CC1101)"; // Indicates if CC1101 is connected
    else txt += " Tx: " + String(leleConfig.rfTx) + " Rx: " + String(leleConfig.rfRx);

    loopOptions(options, MENU_TYPE_SUBMENU, txt.c_str());
}

void RFMenu::configMenu() {
    options = {
        {"RF TX Pin", lambdaHelper(gsetRfTxPin, true)},
        {"RF RX Pin", lambdaHelper(gsetRfRxPin, true)},
        {"RF Module", setRFModuleMenu},
        {"RF Frequency", setRFFreqMenu},
        {"Voltar", [this]() { optionsMenu(); }},
    };

    loopOptions(options, MENU_TYPE_SUBMENU, "RF Config");
}
void RFMenu::drawIconImg() {
    drawImg(
        *leleConfig.themeFS(), leleConfig.getThemeItemImg(leleConfig.theme.paths.rf), 0, imgCenterY, true
    );
}
void RFMenu::drawIcon(float scale) {
    clearIconArea();
    int radius = scale * 7;
    int deltaRadius = scale * 10;
    int triangleSize = scale * 30;

    if (triangleSize % 2 != 0) triangleSize++;

    // Body
    tft.fillCircle(iconCenterX, iconCenterY - radius, radius, leleConfig.priColor);
    tft.fillTriangle(
        iconCenterX,
        iconCenterY,
        iconCenterX - triangleSize / 2,
        iconCenterY + triangleSize,
        iconCenterX + triangleSize / 2,
        iconCenterY + triangleSize,
        leleConfig.priColor
    );

    // Left Arcs
    tft.drawArc(
        iconCenterX,
        iconCenterY - radius,
        2.5 * radius,
        2 * radius,
        40,
        140,
        leleConfig.priColor,
        leleConfig.bgColor
    );
    tft.drawArc(
        iconCenterX,
        iconCenterY - radius,
        2.5 * radius + deltaRadius,
        2 * radius + deltaRadius,
        40,
        140,
        leleConfig.priColor,
        leleConfig.bgColor
    );
    tft.drawArc(
        iconCenterX,
        iconCenterY - radius,
        2.5 * radius + 2 * deltaRadius,
        2 * radius + 2 * deltaRadius,
        40,
        140,
        leleConfig.priColor,
        leleConfig.bgColor
    );

    // Right Arcs
    tft.drawArc(
        iconCenterX,
        iconCenterY - radius,
        2.5 * radius,
        2 * radius,
        220,
        320,
        leleConfig.priColor,
        leleConfig.bgColor
    );
    tft.drawArc(
        iconCenterX,
        iconCenterY - radius,
        2.5 * radius + deltaRadius,
        2 * radius + deltaRadius,
        220,
        320,
        leleConfig.priColor,
        leleConfig.bgColor
    );
    tft.drawArc(
        iconCenterX,
        iconCenterY - radius,
        2.5 * radius + 2 * deltaRadius,
        2 * radius + 2 * deltaRadius,
        220,
        320,
        leleConfig.priColor,
        leleConfig.bgColor
    );
}
