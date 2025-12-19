/**
 * @file HardwareMenu.cpp
 * @brief Menu de Hardware do Sistema
 * @author Lele Origin Team
 * @version 1.0
 *
 * Submenus:
 * - Módulos: Lista de módulos de hardware conectados
 * - Status: Estado atual dos módulos
 * - Conflitos: Detecção de conflitos de pinos
 * - GPIOs: Configuração de pinos GPIO
 * - Detectar: Auto-detecção de hardware
 * - Reset HW: Reset de configurações de hardware
 */

#include "HardwareMenu.h"
#include "core/display.h"
#include "core/i2c_finder.h"
#include "core/settings.h"
#include "core/utils.h"
#include <WiFi.h>
#include <globals.h>


// Estrutura para módulos de hardware
struct HardwareModule {
    const char *name;
    bool (*detectFunc)();
    bool enabled;
    const char *pins;
};

// Função para verificar CC1101
static bool detectCC1101() { return leleConfig.rfModule == CC1101_SPI_MODULE; }

// Função para verificar NRF24
static bool detectNRF24() { return leleConfigPins.NRF24_bus.mosi != GPIO_NUM_NC; }

// Função para verificar PN532
static bool detectPN532() { return leleConfig.rfidModule != 0; }

// Função para verificar GPS
static bool detectGPS() { return leleConfigPins.gps_bus.tx != GPIO_NUM_NC; }

// Função para verificar SD Card
static bool detectSDCard() { return setupSdCard(); }

void HardwareMenu::optionsMenu() {
    options.clear();

    options.push_back({"Modulos", [this]() { modulesMenu(); }});
    options.push_back({"Status", [this]() { statusMenu(); }});
    options.push_back({"Conflitos", [this]() { conflictsMenu(); }});
    options.push_back({"GPIOs", [this]() { gpiosMenu(); }});
    options.push_back({"Detectar", [this]() { detectMenu(); }});
    options.push_back({"Reset HW", [this]() { resetHwMenu(); }});

    addOptionToMainMenu();
    loopOptions(options, MENU_TYPE_SUBMENU, "Hardware");
}

void HardwareMenu::modulesMenu() {
    options.clear();

    // Lista de módulos com status
    options.push_back({String("CC1101: ") + (detectCC1101() ? "Ativo" : "Inativo"), [=]() {
                           if (detectCC1101()) {
                               displayInfo(
                                   "CC1101 Conectado\n\nSCK: " + String((int)leleConfigPins.CC1101_bus.sck) +
                                       "\nMISO: " + String((int)leleConfigPins.CC1101_bus.miso) +
                                       "\nMOSI: " + String((int)leleConfigPins.CC1101_bus.mosi) +
                                       "\nCS: " + String((int)leleConfigPins.CC1101_bus.cs),
                                   true
                               );
                           } else {
                               displayWarning("CC1101 nao detectado\n\nVerifique conexoes", true);
                           }
                       }});

    options.push_back({String("NRF24: ") + (detectNRF24() ? "Ativo" : "Inativo"), [=]() {
                           if (detectNRF24()) {
                               displayInfo(
                                   "NRF24 Conectado\n\nSCK: " + String((int)leleConfigPins.NRF24_bus.sck) +
                                       "\nMISO: " + String((int)leleConfigPins.NRF24_bus.miso) +
                                       "\nMOSI: " + String((int)leleConfigPins.NRF24_bus.mosi) +
                                       "\nCS: " + String((int)leleConfigPins.NRF24_bus.cs),
                                   true
                               );
                           } else {
                               displayWarning("NRF24 nao detectado", true);
                           }
                       }});

    options.push_back({String("PN532/RFID: ") + (detectPN532() ? "Ativo" : "Inativo"), [=]() {
                           String modType = "Desconhecido";
                           if (leleConfig.rfidModule == M5_RFID2_MODULE) modType = "M5 RFID2";
                           else if (leleConfig.rfidModule == PN532_I2C_MODULE) modType = "PN532 I2C";
                           else if (leleConfig.rfidModule == PN532_SPI_MODULE) modType = "PN532 SPI";
                           else if (leleConfig.rfidModule == RC522_SPI_MODULE) modType = "RC522 SPI";

                           displayInfo("RFID: " + modType, true);
                       }});

    options.push_back({String("GPS: ") + (detectGPS() ? "Ativo" : "Inativo"), [=]() {
                           if (detectGPS()) {
                               displayInfo(
                                   "GPS Configurado\n\nTX: " + String((int)leleConfigPins.gps_bus.tx) +
                                       "\nRX: " + String((int)leleConfigPins.gps_bus.rx) +
                                       "\nBaud: " + String(leleConfig.gpsBaudrate),
                                   true
                               );
                           } else {
                               displayWarning("GPS nao configurado", true);
                           }
                       }});

    options.push_back(
        {String("SD Card: ") + (detectSDCard() ? "OK" : "Erro"), [=]() {
             if (detectSDCard()) {
                 uint64_t cardSize = SD.cardSize() / (1024 * 1024);
                 displayInfo("SD Card OK\n\nTamanho: " + String((uint32_t)cardSize) + " MB", true);
             } else {
                 displayWarning("SD Card nao encontrado", true);
             }
         }}
    );

    options.push_back({"I2C Scan", [=]() { find_i2c_addresses(); }});

    options.push_back({"Voltar", [this]() { optionsMenu(); }});

    loopOptions(options, MENU_TYPE_SUBMENU, "Modulos HW");
}

void HardwareMenu::statusMenu() {
    options.clear();

    // Informações do sistema
    options.push_back({"Chip Info", [=]() {
                           String info = "ESP32\n\n";
                           info += "Cores: " + String(ESP.getChipCores()) + "\n";
                           info += "Freq: " + String(ESP.getCpuFreqMHz()) + " MHz\n";
                           info += "Flash: " + String(ESP.getFlashChipSize() / 1024 / 1024) + " MB\n";
                           info += "Free Heap: " + String(ESP.getFreeHeap() / 1024) + " KB";
                           displayInfo(info, true);
                       }});

    options.push_back({"Memoria", [=]() {
                           String info = "Memoria\n\n";
                           info += "Heap Total: " + String(ESP.getHeapSize() / 1024) + " KB\n";
                           info += "Heap Livre: " + String(ESP.getFreeHeap() / 1024) + " KB\n";
                           info += "PSRAM: " + String(ESP.getPsramSize() / 1024) + " KB\n";
                           info += "PSRAM Livre: " + String(ESP.getFreePsram() / 1024) + " KB";
                           displayInfo(info, true);
                       }});

    options.push_back({"Display", [=]() {
                           String info = "Display\n\n";
                           info += "Largura: " + String(tftWidth) + " px\n";
                           info += "Altura: " + String(tftHeight) + " px\n";
                           info += "Rotacao: " + String(leleConfig.rotation);
                           displayInfo(info, true);
                       }});

    options.push_back({"WiFi MAC", [=]() { displayInfo("WiFi MAC:\n\n" + WiFi.macAddress(), true); }});

    options.push_back({"Uptime", [=]() {
                           unsigned long secs = millis() / 1000;
                           unsigned long mins = secs / 60;
                           unsigned long hours = mins / 60;
                           secs = secs % 60;
                           mins = mins % 60;

                           char buf[32];
                           snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu", hours, mins, secs);
                           displayInfo("Uptime:\n\n" + String(buf), true);
                       }});

    options.push_back({"Voltar", [this]() { optionsMenu(); }});

    loopOptions(options, MENU_TYPE_SUBMENU, "Status Sistema");
}

void HardwareMenu::conflictsMenu() {
    options.clear();

    bool hasConflicts = false;
    String conflictList = "";

    // Verifica conflitos entre SPI buses
    if (leleConfigPins.CC1101_bus.mosi == leleConfigPins.SDCARD_bus.mosi &&
        leleConfigPins.CC1101_bus.mosi != GPIO_NUM_NC) {
        conflictList += "CC1101 e SD compartilham SPI\n";
    }

    if (leleConfigPins.NRF24_bus.mosi == leleConfigPins.SDCARD_bus.mosi &&
        leleConfigPins.NRF24_bus.mosi != GPIO_NUM_NC) {
        conflictList += "NRF24 e SD compartilham SPI\n";
    }

    // Verifica conflitos UART/GPS
    if (leleConfigPins.uart_bus.tx == leleConfigPins.gps_bus.tx &&
        leleConfigPins.uart_bus.tx != GPIO_NUM_NC) {
        conflictList += "UART e GPS compartilham TX\n";
        hasConflicts = true;
    }

    options.push_back({"Verificar Conflitos", [=]() {
                           if (conflictList.length() > 0) {
                               displayWarning("Conflitos:\n\n" + conflictList, true);
                           } else {
                               displaySuccess("Nenhum conflito\ndetectado!", true);
                           }
                       }});

    options.push_back({"CC1101 vs SD", [=]() {
                           bool shared =
                               (leleConfigPins.CC1101_bus.mosi == leleConfigPins.SDCARD_bus.mosi &&
                                leleConfigPins.CC1101_bus.mosi != GPIO_NUM_NC);
                           if (shared) {
                               displayWarning(
                                   "CC1101 e SD Card\ncompartilham barramento SPI\n\nPode causar conflitos",
                                   true
                               );
                           } else {
                               displaySuccess("CC1101 e SD Card\nusam barramentos separados", true);
                           }
                       }});

    options.push_back({"NRF24 vs SD", [=]() {
                           bool shared =
                               (leleConfigPins.NRF24_bus.mosi == leleConfigPins.SDCARD_bus.mosi &&
                                leleConfigPins.NRF24_bus.mosi != GPIO_NUM_NC);
                           if (shared) {
                               displayWarning("NRF24 e SD Card\ncompartilham barramento SPI", true);
                           } else {
                               displaySuccess("NRF24 e SD Card\nusam barramentos separados", true);
                           }
                       }});

    options.push_back({"Voltar", [this]() { optionsMenu(); }});

    loopOptions(options, MENU_TYPE_SUBMENU, "Conflitos HW");
}

void HardwareMenu::gpiosMenu() {
    options.clear();

    options.push_back({"SPI CC1101", [=]() { setSPIPinsMenu(leleConfigPins.CC1101_bus); }});

    options.push_back({"SPI NRF24", [=]() { setSPIPinsMenu(leleConfigPins.NRF24_bus); }});

    options.push_back({"SPI SD Card", [=]() { setSPIPinsMenu(leleConfigPins.SDCARD_bus); }});

    options.push_back({"I2C", [=]() { setI2CPinsMenu(leleConfigPins.i2c_bus); }});

    options.push_back({"UART", [=]() { setUARTPinsMenu(leleConfigPins.uart_bus); }});

    options.push_back({"GPS", [=]() { setUARTPinsMenu(leleConfigPins.gps_bus); }});

    options.push_back({"IR TX", [=]() { gsetIrTxPin(true); }});

    options.push_back({"IR RX", [=]() { gsetIrRxPin(true); }});

    options.push_back({"RF TX", [=]() { gsetRfTxPin(true); }});

    options.push_back({"RF RX", [=]() { gsetRfRxPin(true); }});

    options.push_back({"Voltar", [this]() { optionsMenu(); }});

    loopOptions(options, MENU_TYPE_SUBMENU, "Configurar GPIOs");
}

void HardwareMenu::detectMenu() {
    options.clear();

    options.push_back({"Auto-Detectar Tudo", [=]() {
                           displayInfo("Detectando hardware...", false);
                           delay(500);

                           String result = "Detectado:\n\n";
                           int count = 0;

                           if (detectCC1101()) {
                               result += "- CC1101\n";
                               count++;
                           }
                           if (detectNRF24()) {
                               result += "- NRF24\n";
                               count++;
                           }
                           if (detectPN532()) {
                               result += "- RFID/NFC\n";
                               count++;
                           }
                           if (detectGPS()) {
                               result += "- GPS\n";
                               count++;
                           }
                           if (detectSDCard()) {
                               result += "- SD Card\n";
                               count++;
                           }

                           result += "\nTotal: " + String(count) + " modulos";
                           displayInfo(result, true);
                       }});

    options.push_back({"Scan I2C", [=]() { find_i2c_addresses(); }});

    options.push_back({"Testar CC1101", [=]() {
                           if (detectCC1101()) {
                               displaySuccess("CC1101 OK!", true);
                           } else {
                               displayError("CC1101 nao encontrado", true);
                           }
                       }});

    options.push_back({"Testar NRF24", [=]() {
                           if (detectNRF24()) {
                               displaySuccess("NRF24 configurado!", true);
                           } else {
                               displayWarning("NRF24 sem pinos definidos", true);
                           }
                       }});

    options.push_back({"Testar SD Card", [=]() {
                           if (detectSDCard()) {
                               displaySuccess("SD Card OK!", true);
                           } else {
                               displayError("SD Card nao encontrado", true);
                           }
                       }});

    options.push_back({"Voltar", [this]() { optionsMenu(); }});

    loopOptions(options, MENU_TYPE_SUBMENU, "Detectar Hardware");
}

void HardwareMenu::resetHwMenu() {
    options.clear();

    options.push_back({"Reset Pinos SPI", [=]() {
                           displayWarning("Resetar pinos SPI\npara padrao?\n\nSel: Confirmar", false);
                           while (1) {
                               if (check(SelPress)) {
                                   // Reset para valores padrão
                                   displaySuccess("Pinos SPI resetados!\nReinicie o dispositivo.", true);
                                   break;
                               }
                               if (check(EscPress) || check(AnyKeyPress)) {
                                   displayInfo("Cancelado", true);
                                   break;
                               }
                               delay(10);
                           }
                       }});

    options.push_back({"Reset Pinos I2C", [=]() {
                           displayWarning("Resetar pinos I2C\npara padrao?", true);
                       }});

    options.push_back({"Reset Pinos UART", [=]() {
                           displayWarning("Resetar pinos UART\npara padrao?", true);
                       }});

    options.push_back(
        {"Reset Todos Pinos", [=]() {
             displayWarning("ATENCAO!\n\nIsso vai resetar\nTODOS os pinos!\n\nSel: Confirmar", false);
             while (1) {
                 if (check(SelPress)) {
                     leleConfigPins.saveFile();
                     displaySuccess("Pinos resetados!\nReinicie o dispositivo.", true);
                     break;
                 }
                 if (check(EscPress) || check(AnyKeyPress)) {
                     displayInfo("Cancelado", true);
                     break;
                 }
                 delay(10);
             }
         }}
    );

    options.push_back({"Voltar", [this]() { optionsMenu(); }});

    loopOptions(options, MENU_TYPE_SUBMENU, "Reset Hardware");
}

void HardwareMenu::drawIconImg() {
    drawImg(
        *leleConfig.themeFS(), leleConfig.getThemeItemImg(leleConfig.theme.paths.config), 0, imgCenterY, true
    );
}

void HardwareMenu::drawIcon(float scale) {
    clearIconArea();

    // Desenha ícone de chip/circuito
    int chipW = scale * 40;
    int chipH = scale * 30;
    int pinLen = scale * 8;
    int pinW = scale * 3;
    int numPins = 4;

    int chipX = iconCenterX - chipW / 2;
    int chipY = iconCenterY - chipH / 2;

    // Corpo do chip
    tft.fillRect(chipX, chipY, chipW, chipH, leleConfig.bgColor);
    tft.drawRect(chipX, chipY, chipW, chipH, leleConfig.priColor);

    // Pinos superiores
    for (int i = 0; i < numPins; i++) {
        int px = chipX + chipW / (numPins + 1) * (i + 1) - pinW / 2;
        tft.fillRect(px, chipY - pinLen, pinW, pinLen, leleConfig.priColor);
    }

    // Pinos inferiores
    for (int i = 0; i < numPins; i++) {
        int px = chipX + chipW / (numPins + 1) * (i + 1) - pinW / 2;
        tft.fillRect(px, chipY + chipH, pinW, pinLen, leleConfig.priColor);
    }

    // Pinos laterais
    for (int i = 0; i < 2; i++) {
        int py = chipY + chipH / 3 * (i + 1) - pinW / 2;
        tft.fillRect(chipX - pinLen, py, pinLen, pinW, leleConfig.priColor);
        tft.fillRect(chipX + chipW, py, pinLen, pinW, leleConfig.priColor);
    }

    // Marca do chip (canto)
    tft.fillCircle(chipX + scale * 5, chipY + scale * 5, scale * 2, leleConfig.priColor);
}
