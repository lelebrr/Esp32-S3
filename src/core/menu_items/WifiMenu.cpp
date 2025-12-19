#include "WifiMenu.h"
#include <lvgl.h>
#include "core/display.h"
#include "core/settings.h"
#include "core/utils.h"
#include "core/wifi/webInterface.h"
#include "core/wifi/wg.h"
#include "core/wifi/wifi_common.h"
#include "core/wifi/wifi_mac.h"
#include "modules/ble/ble_flood.h"
#include "modules/ethernet/ARPScanner.h"
#include "modules/wifi/ap_info.h"
#include "modules/wifi/clients.h"
#include "modules/wifi/evil_portal.h"
#include "modules/wifi/iot_deauth.h"
#include "modules/wifi/karma_attack.h"
#include "modules/wifi/responder.h"
#include "modules/wifi/scan_hosts.h"
#include "modules/wifi/sniffer.h"
#include "modules/wifi/wifi_advanced_attacks.h"
#include "modules/wifi/wifi_atks.h"
#include "modules/wifi/wifi_chaos.h"
#include "modules/wifi/wifi_enterprise.h"
#include "modules/wifi/wifi_handshake_attacks.h"
#include "modules/wifi/wifite_auto.h"
#include "modules/wps/wps_attacks.h"
#include "modules/wifi/wifi_mitm_attacks.h"

extern "C" void wpsMenuFunction();

#ifndef LITE_VERSION
#include "modules/pwnagotchi/pwnagotchi.h"
#endif

// #include "modules/reverseShell/reverseShell.h"
//  Developed by Fourier (github.com/9dl)
//  Use LeleC2 to interact with the reverse shell server
//  LeleC2: https://github.com/9dl/Lele-C2
//  To use LeleC2:
//  1. Start Reverse Shell Mode in Lele
//  2. Start LeleC2 and wait.
//  3. Visit 192.168.4.1 in your browser to access the web interface for shell executing.

// 32bit: https://github.com/9dl/Lele-C2/releases/download/v1.0/LeleC2_windows_386.exe
// 64bit: https://github.com/9dl/Lele-C2/releases/download/v1.0/LeleC2_windows_amd64.exe
#include "modules/wifi/tcp_utils.h"

// global toggle - controls whether scanNetworks includes hidden SSIDs
bool showHiddenNetworks = false;

void WifiMenu::optionsMenu() {
    returnToMenu = false;
    options.clear();
    if (isWebUIActive) {
        drawMainBorderWithTitle("WiFi", true);
        padprintln("");
        padprintln("Iniciar uma fun��o WiFi provavelmente far� a WebUI parar de funcionar");
        padprintln("");
        padprintln("Sel: para continuar");
        padprintln("Qualquer tecla: Menu");
        while (1) {
            if (check(SelPress)) { break; }
            if (check(AnyKeyPress)) { return; }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
    if (WiFi.status() != WL_CONNECTED) {
        options = {
            {"Conectar WiFi", lambdaHelper(wifiConnectMenu, WIFI_STA)},
            {"Iniciar WiFi AP", [=]() {
                 wifiConnectMenu(WIFI_AP);
                 displayInfo("pwd: " + leleConfig.wifiAp.pwd, true);
             }},
        };
    }
    if (WiFi.getMode() != WIFI_MODE_NULL) { options.push_back({"Desligar WiFi", wifiDisconnect}); }
    if (WiFi.getMode() == WIFI_MODE_STA || WiFi.getMode() == WIFI_MODE_APSTA) {
        options.push_back({"Info do AP", displayAPInfo});
    }
    options.push_back({"Ataques WiFi", wifi_atk_menu});
    options.push_back({"Portal Malicioso", [=]() {
                           if (isWebUIActive || server) {
                               stopWebUi();
                               wifiDisconnect();
                           }
                           EvilPortal();
                       }});
    options.push_back({"MITM Attacks", [this]() {
                           std::vector<Option> mitmOptions;
                           mitmOptions.push_back({"DNS Spoof", [=]() { wifi_mitm_dnsspoof(); }});
                           mitmOptions.push_back({"HTTPS Downgrade", [=]() { wifi_https_downgrade(); }});
                           mitmOptions.push_back({"Captive Survey", [=]() { wifi_captive_portal_survey(); }});
                           mitmOptions.push_back({"Beacon Spam", [=]() { wifi_beacon_spam(); }});
                           mitmOptions.push_back({"Spotify OAuth", [=]() { wifi_spotify_oauth_portal(); }});
                           mitmOptions.push_back({"Voltar", [this]() { optionsMenu(); }});
                           loopOptions(mitmOptions, MENU_TYPE_SUBMENU, "MITM Attacks");
                       }});
    // options.push_back({"ReverseShell", [=]()       { ReverseShell(); }});
    options.push_back({"Escutar TCP", listenTcpPort});
    options.push_back({"Cliente TCP", clientTCP});
#ifndef LITE_VERSION
    options.push_back({"TelNET", telnet_setup});
    options.push_back({"SSH", lambdaHelper(ssh_setup, String(""))});
    options.push_back({"Analisadores", [this]() {
                           std::vector<Option> snifferOptions;
                           snifferOptions.push_back({"Captura RAW", sniffer_setup});
                           snifferOptions.push_back({"Captura Probe", karma_setup});
                           snifferOptions.push_back({"Voltar", [this]() { optionsMenu(); }});

                           loopOptions(snifferOptions, MENU_TYPE_SUBMENU, "Analisadores");
                       }});
    options.push_back({"Escanear Hosts", [=]() {
                           bool doScan = true;
                           if (!wifiConnected) doScan = wifiConnectMenu();

                           if (doScan) {
                               esp_netif_t *esp_netinterface =
                                   esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
                               if (esp_netinterface == nullptr) {
                                   Serial.println("Failed to get netif handle");
                                   return;
                               }
                               ARPScanner{esp_netinterface};
                           }
                       }});
    options.push_back({"Wireguard", wg_setup});
    options.push_back({"Respondedor", responder});
    options.push_back({"Lelegotchi", Lelegotchi_start});
    options.push_back({"Wifite Mode", wifite_menu});
    options.push_back({"Hacking WiFi", [this]() { advancedAttacksMenu(); }});
#endif
    options.push_back({"Configurar", [this]() { configMenu(); }});
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "WiFi");
}

void WifiMenu::configMenu() {
    std::vector<Option> wifiOptions;

    wifiOptions.push_back({"Alterar MAC", wifiMACMenu});
    wifiOptions.push_back({"Adicionar WiFi Malicioso", addEvilWifiMenu});
    wifiOptions.push_back({"Remover WiFi Malicioso", removeEvilWifiMenu});

    // Evil Wifi Settings submenu (unchanged)
    wifiOptions.push_back({"Config WiFi Malicioso", [this]() {
                               std::vector<Option> evilOptions;

                               evilOptions.push_back({"Modo Senha", setEvilPasswordMode});
                               evilOptions.push_back({"Rename /creds", setEvilEndpointCreds});
                               evilOptions.push_back({"Allow /creds access", setEvilAllowGetCreds});
                               evilOptions.push_back({"Rename /ssid", setEvilEndpointSsid});
                               evilOptions.push_back({"Allow /ssid access", setEvilAllowSetSsid});
                               evilOptions.push_back({"Display endpoints", setEvilAllowEndpointDisplay});
                               evilOptions.push_back({"Voltar", [this]() { configMenu(); }});
                               loopOptions(evilOptions, MENU_TYPE_SUBMENU, "Config WiFi Malicioso");
                           }});

    // NEW: Show Hidden Networks toggle
    {
        // build the label showing current state
        std::string label = std::string("Mostrar Redes Ocultas: ") + (showHiddenNetworks ? "ON" : "OFF");

        // construct Option explicitly using char* label
        Option opt(label.c_str(), [this]() {
            // toggle the global flag
            showHiddenNetworks = !showHiddenNetworks;
            // immediate feedback
            displayInfo(String("Mostrar Redes Ocultas: ") + (showHiddenNetworks ? "ON" : "OFF"), true);
            // refresh menu so the label updates
            configMenu();
        });

        wifiOptions.push_back(opt);
    }
    wifiOptions.push_back({"Voltar", [this]() { optionsMenu(); }});
    loopOptions(wifiOptions, MENU_TYPE_SUBMENU, "WiFi Config");
}

void WifiMenu::drawIconImg() {
    drawImg(
        *leleConfig.themeFS(), leleConfig.getThemeItemImg(leleConfig.theme.paths.wifi), 0, imgCenterY, true
    );
}
void WifiMenu::drawIcon(float scale) {
    clearIconArea();
    int deltaY = scale * 20;
    int radius = scale * 6;

    tft.fillCircle(iconCenterX, iconCenterY + deltaY, radius, leleConfig.priColor);
    tft.drawArc(
        iconCenterX,
        iconCenterY + deltaY,
        deltaY + radius,
        deltaY,
        130,
        230,
        leleConfig.priColor,
        leleConfig.bgColor
    );
    tft.drawArc(
        iconCenterX,
        iconCenterY + deltaY,
        2 * deltaY + radius,
        2 * deltaY,
        130,
        230,
        leleConfig.priColor,
        leleConfig.bgColor
    );
}

/**
 * @brief Menu de ataques WiFi avançados
 *
 * Submenu com as seguintes opções:
 * - Ataques WPS: Scan, Pixie Dust, PIN Flood
 * - Deauth IoT: Chromecast, Alexa, Smart TVs
 * - Beacon Spam: Rickroll, Nomes Aleatórios
 * - One Tap Nuke: Combo de todos os ataques
 */
void WifiMenu::advancedAttacksMenu() {
    std::vector<Option> advOptions;

    // Novo menu WPS com Pixie Dust + Reaver
    advOptions.push_back({"WPS Attacks", [=]() { wpsMenuFunction(); }});

    // Submenu IoT Deauth
    advOptions.push_back({"Deauth IoT", [this]() {
                              std::vector<Option> iotOptions;
                              iotOptions.push_back({"Chromecast", [=]() {
                                                        iot_deauth_init();
                                                        int count = iot_deauth_scan(IOT_TARGET_CHROMECAST);
                                                        if (count > 0) iot_deauth_start();
                                                    }});
                              iotOptions.push_back({"Alexa/Echo", [=]() {
                                                        iot_deauth_init();
                                                        int count = iot_deauth_scan(IOT_TARGET_ALEXA);
                                                        if (count > 0) iot_deauth_start();
                                                    }});
                              iotOptions.push_back({"Smart TVs", [=]() {
                                                        iot_deauth_init();
                                                        int count = iot_deauth_scan(IOT_TARGET_SMARTTV);
                                                        if (count > 0) iot_deauth_start();
                                                    }});
                              iotOptions.push_back({"Todos IoT", [=]() {
                                                        iot_deauth_init();
                                                        int count = iot_deauth_scan(IOT_TARGET_ALL);
                                                        if (count > 0) iot_deauth_start();
                                                    }});
                              iotOptions.push_back({"Parar Ataque", [=]() { iot_deauth_stop(); }});
                              iotOptions.push_back({"Voltar", [this]() { advancedAttacksMenu(); }});
                              loopOptions(iotOptions, MENU_TYPE_SUBMENU, "Deauth IoT");
                          }});

    // Submenu Beacon Spam
    advOptions.push_back(
        {"Beacon Spam", [this]() {
             std::vector<Option> beaconOptions;
             beaconOptions.push_back({"Rickroll", [=]() { displayInfo("Never gonna\ngive you up!", true); }});
             beaconOptions.push_back({"Nomes Aleatorios", [=]() { displayInfo("Criando caos...", true); }});
             beaconOptions.push_back({"Voltar", [this]() { advancedAttacksMenu(); }});
             loopOptions(beaconOptions, MENU_TYPE_SUBMENU, "Beacon Spam");
         }}
    );

    // One Tap Nuke - Combo de todos os ataques
    advOptions.push_back({"One Tap Nuke", [=]() {
                              displayWarning("INICIANDO NUKE!\nDeauth + BLE Flood", true);
                              delay(1000);

                              // Inicia IoT Deauth em todos os alvos
                              iot_deauth_init();
                              iot_deauth_scan(IOT_TARGET_ALL);
                              iot_deauth_start();

                              // Inicia BLE Flood em todos os modos
                              ble_flood_init();
                              ble_flood_start(BLE_FLOOD_ALL);

                              displayInfo("NUKE ATIVO!\nPressione qualquer\ntecla para parar", false);

                              // Loop até pressionar tecla
                              while (!check(AnyKeyPress)) {
                                  iot_deauth_update();
                                  ble_flood_update();
                                  delay(10);
                              }

                              // Para tudo
                              iot_deauth_stop();
                              ble_flood_stop();
                          }});

    // Submenus consolidados
    advOptions.push_back({"CVE/DoS Attacks", [this]() { cveAttacksMenu(); }});
    advOptions.push_back({"Captura (PMKID)", [this]() { captureAttacksMenu(); }});
    advOptions.push_back({"Chaos Mode", [this]() { wifi_chaos_menu(); }});           // NOVO
    advOptions.push_back({"Enterprise Test", [this]() { wifi_enterprise_menu(); }}); // NOVO

    advOptions.push_back({"Voltar", [this]() { optionsMenu(); }});
    loopOptions(advOptions, MENU_TYPE_SUBMENU, "Hacking WiFi");
}

/**
 * @brief Menu de Ataques CVE 2024 (DoS/Flood)
 */
void WifiMenu::cveAttacksMenu() {
    std::vector<Option> cveOptions;

    wifi_adv_init();

    cveOptions.push_back({"Escanear Alvos", [=]() { wifi_adv_scan(5000); }});

    cveOptions.push_back({"--- CVE-BASED ---", [=]() {}});

    cveOptions.push_back({"MediaTek DoS (CVE-2024)", [=]() {
                              uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                              wifi_adv_mediatek_dos(bcast, 6);
                          }});

    cveOptions.push_back({"SSID Confusion (CVE-2023)", [=]() { wifi_adv_ssid_confusion("TARGET", 6); }});

    cveOptions.push_back({"FragAttacks 2024", [=]() {
                              uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                              wifi_adv_fragattacks(bcast, 6);
                          }});

    cveOptions.push_back({"--- FLOOD ---", [=]() {}});

    cveOptions.push_back({"Auth Flood DoS", [=]() {
                              uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                              wifi_adv_auth_flood(bcast, 6, 15000);
                          }});

    cveOptions.push_back({"Disassoc Flood", [=]() {
                              uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                              wifi_adv_disassoc_flood(bcast, 6);
                          }});

    cveOptions.push_back({"Beacon Fuzzing", [=]() { wifi_adv_beacon_fuzzing(6); }});

    cveOptions.push_back({"Channel Hop Deauth", [=]() { wifi_adv_channel_hopping_deauth(NULL); }});

    cveOptions.push_back({"Mgmt Frame Crash", [=]() {
                              uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                              wifi_adv_mgmt_frame_crash(bcast, 6);
                          }});

    cveOptions.push_back({"Loud Beacon", [=]() { wifi_adv_loud_beacon(6); }});

    cveOptions.push_back({"Parar Ataques", [=]() { wifi_adv_stop_all(); }});

    cveOptions.push_back({"Voltar", [this]() { optionsMenu(); }});
    loopOptions(cveOptions, MENU_TYPE_SUBMENU, "Ataques CVE 2024");
}

/**
 * @brief Menu de Captura Avancada (PMKID/Handshake)
 */
void WifiMenu::captureAttacksMenu() {
    std::vector<Option> capOptions;

    wifi_cap_init();

    capOptions.push_back({"--- PMKID ---", [=]() {}});

    capOptions.push_back({"PMKID Clientless", [=]() {
                              uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                              wifi_cap_pmkid_clientless(bcast, 6);
                          }});

    capOptions.push_back({"Multi-Target PMKID", [=]() { wifi_cap_multi_target_pmkid(); }});

    capOptions.push_back({"Fast Roaming PMKID", [=]() {
                              uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                              wifi_cap_fast_roaming_pmkid(bcast, 6);
                          }});

    capOptions.push_back({"--- HANDSHAKE ---", [=]() {}});

    capOptions.push_back({"Handshake Passivo", [=]() {
                              uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                              wifi_cap_passive_handshake(bcast, 6);
                          }});

    capOptions.push_back({"Deauth + Capture", [=]() {
                              uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                              wifi_cap_deauth_handshake(bcast, NULL, 6);
                          }});

    capOptions.push_back({"Deauth Storm Multi", [=]() {
                              uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                              wifi_cap_deauth_storm_multi(bcast, 6);
                          }});

    capOptions.push_back({"Channel Hop Sniff", [=]() { wifi_cap_channel_hop_sniff(); }});

    capOptions.push_back({"--- DOWNGRADE ---", [=]() {}});

    capOptions.push_back({"Evil Twin Downgrade", [=]() { wifi_cap_evil_twin_downgrade("TARGET", 6); }});

    capOptions.push_back({"SAE to PSK", [=]() { wifi_cap_sae_to_psk("TARGET", 6); }});

    capOptions.push_back({"--- CONTROLE ---", [=]() {}});

    capOptions.push_back({"Salvar no SD", [=]() { wifi_cap_save_to_sd("/wifi_captures.txt"); }});

    capOptions.push_back({"Limpar Capturas", [=]() { wifi_cap_clear(); }});

    capOptions.push_back({"Parar Captura", [=]() { wifi_cap_stop(); }});

    capOptions.push_back({"Voltar", [this]() { optionsMenu(); }});
    loopOptions(capOptions, MENU_TYPE_SUBMENU, "Captura Avancada");
}
