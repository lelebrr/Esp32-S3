#include "RFIDMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/utils.h"
#include "modules/rfid/PN532KillerTools.h"
#include "modules/rfid/amiibo.h"
#include "modules/rfid/chameleon.h"
#include "modules/rfid/pn532ble.h"
#include "modules/rfid/rfid125.h"
#include "modules/rfid/rfid_fuzzer.h"
#include "modules/rfid/tag_o_matic.h"
#include "modules/rfid/nfc_attacks.h"
#ifndef LITE_VERSION
#include "modules/rfid/emv_reader.hpp"
#endif
void RFIDMenu::optionsMenu() {
    options = {
#if !defined(REMOVE_RFID_HW_INTERFACE)  // Remove Hardware interface menu due to lack of external GPIO
        {"Ler tag",          [=]() { TagOMatic(); }                          },
#ifndef LITE_VERSION
        {"Read EMV",         [=]() { EMVReader(); }                          },
#endif
        {"Ler 125kHz",       [=]() { RFID125(); }                            },
        {"Fuzzer 125kHz",    [=]() { rfid_fuzzer_run(); }                    }, // NOVO
        {"Scan tags",        [=]() { TagOMatic(TagOMatic::SCAN_MODE); }      },
        {"Carregar arquivo", [=]() { TagOMatic(TagOMatic::LOAD_MODE); }      },
        {"Apagar dados",     [=]() { TagOMatic(TagOMatic::ERASE_MODE); }     },
        {"Gravar NDEF",      [=]() { TagOMatic(TagOMatic::WRITE_NDEF_MODE); }},
        {"--- NFC Attacks ---", [=]() {}                                     },
        {"Clone Card",       [=]() { nfc_clone_card(); }                     },
        {"Phishing Tag",     [=]() { nfc_phishing_tag(); }                   },
        {"OTA Rewrite",      [=]() { nfc_ota_rewrite(); }                    },
        {"Fake Apple Pay",   [=]() { nfc_fake_apple_pay(); }                 },
        {"Audio Injection",  [=]() { nfc_audio_injection(); }                },
        {"--- Pulse Injection ---", [=]() {}                                 },
        {"Claw Machine Credit", [=]() { nfc_pulse_injection_claw_machine(); }},
        {"Time Clock Shock", [=]() { nfc_pulse_injection_time_clock(); }     },
        {"Bus Card Overflow", [=]() { nfc_pulse_injection_bus_card(); }      },
        {"Reverse Credit",   [=]() { nfc_pulse_injection_reverse_credit(); } },
        {"Ghost Reader",     [=]() { nfc_pulse_injection_ghost_reader(); }   },
#endif
#ifndef LITE_VERSION
        {"Amiibolink",       [=]() { Amiibo(); }                             },
#endif
        {"Chameleon",        [=]() { Chameleon(); }                          },
#ifndef LITE_VERSION
        {"PN532 BLE",        [=]() { Pn532ble(); }                           },
#if !defined(REMOVE_RFID_HW_INTERFACE)  // Remove Hardware interface menu due to lack of external GPIO
        {"PN532 UART",       [=]() { PN532KillerTools(); }                   },
#endif
#endif
        {"NFC Skimmer",      [=]() { nfc_skimmer_attack(); }                  },
        {"Configurar",       [this]() { configMenu(); }                      },
    };
    addOptionToMainMenu();

    vTaskDelay(pdMS_TO_TICKS(200));

    String txt = "RFID";
    if (leleConfig.rfidModule == M5_RFID2_MODULE) txt += " (RFID2)";
#ifdef M5STICK
    else if (leleConfig.rfidModule == PN532_I2C_MODULE) txt += " (PN532-G33)";
    else if (leleConfig.rfidModule == PN532_I2C_SPI_MODULE) txt += " (PN532-G36)";
#else
    else if (leleConfig.rfidModule == PN532_I2C_MODULE) txt += " (PN532-I2C)";
#endif
    else if (leleConfig.rfidModule == PN532_SPI_MODULE) txt += " (PN532-SPI)";
    else if (leleConfig.rfidModule == RC522_SPI_MODULE) txt += " (RC522-SPI)";
    loopOptions(options, MENU_TYPE_SUBMENU, txt.c_str());
}

void RFIDMenu::configMenu() {
    options = {
#if !defined(REMOVE_RFID_HW_INTERFACE)  // Remove Hardware interface menu due to lack of external GPIO
        {"RFID Module", setRFIDModuleMenu          },
#endif
        {"Add MIF Key", addMifareKeyMenu           },
        {"Voltar",      [this]() { optionsMenu(); }},
    };

    loopOptions(options, MENU_TYPE_SUBMENU, "RFID Config");
}
void RFIDMenu::drawIconImg() {
    drawImg(
        *leleConfig.themeFS(), leleConfig.getThemeItemImg(leleConfig.theme.paths.rfid), 0, imgCenterY, true
    );
}
void RFIDMenu::drawIcon(float scale) {
    clearIconArea();
    int iconSize = scale * 70;
    int iconRadius = scale * 7;
    int deltaRadius = scale * 10;

    if (iconSize % 2 != 0) iconSize++;

    tft.drawRoundRect(
        iconCenterX - iconSize / 2,
        iconCenterY - iconSize / 2,
        iconSize,
        iconSize,
        iconRadius,
        leleConfig.priColor
    );
    tft.fillRect(iconCenterX - iconSize / 2, iconCenterY, iconSize / 2, iconSize / 2, leleConfig.bgColor);

    tft.drawCircle(
        iconCenterX - iconSize / 2 + deltaRadius,
        iconCenterY + iconSize / 2 - deltaRadius,
        iconRadius,
        leleConfig.priColor
    );

    tft.drawArc(
        iconCenterX - iconSize / 2 + deltaRadius,
        iconCenterY + iconSize / 2 - deltaRadius,
        2.5 * iconRadius,
        2 * iconRadius,
        180,
        270,
        leleConfig.priColor,
        leleConfig.bgColor
    );
    tft.drawArc(
        iconCenterX - iconSize / 2 + deltaRadius,
        iconCenterY + iconSize / 2 - deltaRadius,
        2.5 * iconRadius + deltaRadius,
        2 * iconRadius + deltaRadius,
        180,
        270,
        leleConfig.priColor,
        leleConfig.bgColor
    );
    tft.drawArc(
        iconCenterX - iconSize / 2 + deltaRadius,
        iconCenterY + iconSize / 2 - deltaRadius,
        2.5 * iconRadius + 2 * deltaRadius,
        2 * iconRadius + 2 * deltaRadius,
        180,
        270,
        leleConfig.priColor,
        leleConfig.bgColor
    );
}
