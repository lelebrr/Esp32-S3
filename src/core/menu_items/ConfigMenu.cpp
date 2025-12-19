#include "ConfigMenu.h"
#include "core/boot_hw_scan.h"
#include "core/display.h"
#include "core/i2c_finder.h"
#include "core/main_menu.h"
#include "core/settings.h"
#include "core/utils.h"
#include "core/wifi/wifi_common.h"
#include "modules/sensors/temperature_monitor.h"
#ifdef HAS_RGB_LED
#include "core/led_control.h"
#endif

void ConfigMenu::optionsMenu() {
    options = {
        {"Brilho", setBrightnessMenu},
        {"Tempo Escurecer", setDimmerTimeMenu},
        {"Orientacao", lambdaHelper(gsetRotation, true)},
        {"Cor da UI", setUIColor},
        {"Tema UI", setTheme},
        {String("InstaBoot: " + String(leleConfig.instantBoot ? "Ligado" : "Desligado")),
         [=]() {
             leleConfig.instantBoot = !leleConfig.instantBoot;
             leleConfig.saveFile();
         }},
#ifdef HAS_RGB_LED
        {"Cor LED",
         [=]() {
             beginLed();
             setLedColorConfig();
         }},
        {"Efeito LED",
         [=]() {
             beginLed();
             setLedEffectConfig();
         }},
        {"Brilho LED",
         [=]() {
             beginLed();
             setLedBrightnessConfig();
         }},
        {"LED Piscar", setLedBlinkConfig},
#endif
        {"Som Liga/Desliga", setSoundConfig},
#if defined(HAS_NS4168_SPKR)
        {"Volume Som", setSoundVolume},
#endif
        {"WiFi ao Iniciar", setWifiStartupConfig},
        {"App ao Iniciar", setStartupApp},
        {"Mostrar/Ocultar Apps", []() { mainMenu.hideAppsMenu(); }},
        {"Ativar API BLE", []() { enableBLEAPI(); }},
        {"Credenciais Rede", setNetworkCredsMenu},
        {"BadUSB/BLE", setBadUSBBLEMenu},
        {"Sensor Temperatura", [this]() { temperatureMenu(); }},
        {"Relogio", setClock},
        {"Dormir", setSleepMode},
        {"Reset de Fabrica", [=]() { leleConfig.factoryReset(); }},
        {"Refazer SD Card", [=]() { sd_rebuild_structure(); }},
        {"Reiniciar", [=]() { ESP.restart(); }},
    };

    options.push_back({"Desligar", powerOff});
    options.push_back({"Suspensao", goToDeepSleep});

    if (leleConfig.devMode) options.push_back({"Modo Dev", [this]() { devMenu(); }});

    options.push_back({"Sobre", showDeviceInfo});
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "Configurar");
}

void ConfigMenu::temperatureMenu() {
    TempConfig *cfg = getTempConfig();
    TempSensorStatus status = getSensorStatus();

    String statusStr = "Sensor: " + String(getSensorStatusText());
    if (status == TEMP_SENSOR_OK) { statusStr += " (" + getTemperatureString() + ")"; }

    options = {
        {statusStr,
         [=]() {
             if (status != TEMP_SENSOR_OK) {
                 displayWarning("Sensor nao encontrado!\nVerifique conexao DS18B20", true);
             } else {
                 displayInfo("Temperatura: " + getTemperatureString(), true);
             }
         }                 },
        {String("Temp. Maxima: ") + String((int)cfg->criticalTemp) + " C",
         [=]() {
             options = {
                 {                                                                          "40 C",
         [=]() {
                      setCriticalTemp(40.0f);
                      setWarningTemp(35.0f);
                      displaySuccess("40C definido", true);
                  }},
                 {                                                                          "45 C",
         [=]() {
                      setCriticalTemp(45.0f);
                      setWarningTemp(40.0f);
                      displaySuccess("45C definido", true);
                  }},
                 {                                                                 "50 C",
         [=]() {
                      setCriticalTemp(50.0f);
                      setWarningTemp(45.0f);
                      displaySuccess("50C definido", true);
                  }},
                 {"55 C",
         [=]() {
                      setCriticalTemp(55.0f);
                      setWarningTemp(50.0f);
                      displaySuccess("55C definido", true);
                  }},
                 {"Voltar", [this]() { temperatureMenu(); }},
             };
             loopOptions(options, MENU_TYPE_SUBMENU, "Temp. Maxima");
         }                           },
        {"Tipo de Alerta",
         [=]() {
             options = {
                 {"LED Vermelho",
         [=]() {
                      setAlertType(TEMP_ALERT_LED);
                      displaySuccess("Alerta LED ativado", true);
                  }},
                 {"Aviso na Tela",
         [=]() {
                      setAlertType(TEMP_ALERT_SCREEN);
                      displaySuccess("Alerta Tela ativado", true);
                  }},
                 {"Som (beep)",
         [=]() {
                      setAlertType(TEMP_ALERT_SOUND);
                      displaySuccess("Alerta Som ativado", true);
                  }},
                 {"Todos",
         [=]() {
                      setAlertType(TEMP_ALERT_ALL);
                      displaySuccess("Todos alertas ativos", true);
                  }},
                 {"Voltar", [this]() { temperatureMenu(); }},
             };
             loopOptions(options, MENU_TYPE_SUBMENU, "Tipo Alerta");
         }                          },
        {"Acao ao Atingir",
         [=]() {
             options = {
                 {"Apenas Avisar",
         [=]() {
                      setTempAction(TEMP_ACTION_WARN);
                      displaySuccess("Acao: Avisar", true);
                  }},
                 {"Pausar Operacoes",
         [=]() {
                      setTempAction(TEMP_ACTION_PAUSE);
                      displaySuccess("Acao: Pausar", true);
                  }},
                 {"Desligar Modulos",
         [=]() {
                      setTempAction(TEMP_ACTION_SHUTDOWN);
                      displaySuccess("Acao: Desligar", true);
                  }},
                 {"Voltar", [this]() { temperatureMenu(); }},
             };
             loopOptions(options, MENU_TYPE_SUBMENU, "Acao");
         }                           },
        {String("Na Barra: ") + (cfg->showOnStatusBar ? "Sim" : "Nao"),
         [=]() {
             setShowOnStatusBar(!cfg->showOnStatusBar);
             displayInfo(cfg->showOnStatusBar ? "Exibindo na barra" : "Ocultado da barra", true);
         }                          },
        {"Salvar Config",
         [=]() {
             saveTempConfig();
             displaySuccess("Configuracao salva!", true);
         }},
        {"Voltar", [this]() { optionsMenu(); }                           },
    };

    loopOptions(options, MENU_TYPE_SUBMENU, "Sensor Temperatura");
}

void ConfigMenu::devMenu() {
    options = {
        {"Buscar I2C",        find_i2c_addresses                                  },
        {"Pinos CC1101",      [=]() { setSPIPinsMenu(leleConfigPins.CC1101_bus); }},
        {"Pinos NRF24",       [=]() { setSPIPinsMenu(leleConfigPins.NRF24_bus); } },
        {"Pinos SDCard",      [=]() { setSPIPinsMenu(leleConfigPins.SDCARD_bus); }},
        {"Pinos I2C",         [=]() { setI2CPinsMenu(leleConfigPins.i2c_bus); }   },
        {"Pinos UART",        [=]() { setUARTPinsMenu(leleConfigPins.uart_bus); } },
        {"Pinos GPS",         [=]() { setUARTPinsMenu(leleConfigPins.gps_bus); }  },
        {"Serial USB",
         [=]() {
             USBserial.setSerialOutput(&Serial);
             Serial1.end();
         }                                                                        },
        {"Serial UART",
         [=]() {
             if (leleConfigPins.SDCARD_bus.checkConflict(leleConfigPins.uart_bus.rx) ||
                 leleConfigPins.SDCARD_bus.checkConflict(leleConfigPins.uart_bus.tx)) {
                 sdcardSPI.end();
             }
             if (leleConfigPins.CC1101_bus.checkConflict(leleConfigPins.uart_bus.rx) ||
                 leleConfigPins.CC1101_bus.checkConflict(leleConfigPins.uart_bus.tx) ||
                 leleConfigPins.NRF24_bus.checkConflict(leleConfigPins.uart_bus.rx) ||
                 leleConfigPins.NRF24_bus.checkConflict(leleConfigPins.uart_bus.tx)) {
                 CC_NRF_SPI.end();
             }
             pinMode(leleConfigPins.uart_bus.rx, INPUT);
             pinMode(leleConfigPins.uart_bus.tx, OUTPUT);
             Serial1.begin(115200, SERIAL_8N1, leleConfigPins.uart_bus.rx, leleConfigPins.uart_bus.tx);
             USBserial.setSerialOutput(&Serial1);
         }                                                                        },
        {"Desativar DevMode", [this]() { leleConfig.setDevMode(false); }          },
        {"Voltar",            [this]() { optionsMenu(); }                         },
    };

    loopOptions(options, MENU_TYPE_SUBMENU, "Modo Dev");
}
void ConfigMenu::drawIconImg() {
    drawImg(
        *leleConfig.themeFS(), leleConfig.getThemeItemImg(leleConfig.theme.paths.config), 0, imgCenterY, true
    );
}
void ConfigMenu::drawIcon(float scale) {
    clearIconArea();
    int radius = scale * 9;

    int i = 0;
    for (i = 0; i < 6; i++) {
        tft.drawArc(
            iconCenterX,
            iconCenterY,
            3.5 * radius,
            2 * radius,
            15 + 60 * i,
            45 + 60 * i,
            leleConfig.priColor,
            leleConfig.bgColor,
            true
        );
    }

    tft.drawArc(
        iconCenterX, iconCenterY, 2.5 * radius, radius, 0, 360, leleConfig.priColor, leleConfig.bgColor, false
    );
}
