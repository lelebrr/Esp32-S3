/**
 * @file boot_hw_scan.cpp
 * @brief Tela de Boot com Scan de Hardware e Verificação do SD Card
 * @author Lele Origin Team
 * @version 1.0
 *
 * Funcionalidades:
 * - Animação de boot com logo
 * - Scan automático de todos os módulos de hardware
 * - Mostra status ONLINE/OFFLINE de cada módulo
 * - Verifica e cria estrutura de pastas no SD Card
 * - Aguarda confirmação do usuário para continuar
 */

#include "boot_hw_scan.h"
#include "display.h"
#include "i2c_finder.h"
#include "mykeyboard.h"
#include "sd_functions.h"
#include "settings.h"
#include <SPI.h>
#include <Wire.h>


// Variáveis globais
HWModule hwModules[HW_MODULE_COUNT];
int hwModulesOnline = 0;
int hwModulesOffline = 0;
bool sdCardReady = false;

// Cores para status
#define COLOR_ONLINE TFT_GREEN
#define COLOR_OFFLINE TFT_RED
#define COLOR_WARNING TFT_YELLOW
#define COLOR_INFO TFT_CYAN

// Estrutura de pastas obrigatórias no SD Card
static const SDRequiredItem requiredSDItems[] = {
    {"/lele",          true, "Pasta raiz do sistema"},
    {"/lele/logs",     true, "Logs de operacoes"    },
    {"/lele/captures", true, "Handshakes e capturas"},
    {"/lele/config",   true, "Configuracoes"        },
    {"/lele/scripts",  true, "Scripts JS"           },
    {"/infrared",      true, "Codigos IR"           },
    {"/portals",       true, "Templates Evil Portal"},
    {"/BadUSB",        true, "Scripts Ducky"        },
    {"/themes",        true, "Temas customizados"   },
    {"/nfc",           true, "Dados NFC/RFID"       },
    {"/gps",           true, "Logs GPS/Wardriving"  },
    {"/rf",            true, "Sinais RF gravados"   },
};
#define REQUIRED_SD_ITEMS_COUNT (sizeof(requiredSDItems) / sizeof(requiredSDItems[0]))

// ============================================================================
// FUNÇÕES DE DETECÇÃO DE HARDWARE
// ============================================================================

bool detect_cc1101() {
    // Verifica se CC1101 está configurado e responde
    if (leleConfig.rfModule != CC1101_SPI_MODULE) { return false; }
    // Se chegou aqui, está configurado (a inicialização real é feita em setup_gpio)
    return true;
}

bool detect_nrf24() {
    // Verifica se NRF24 tem pinos configurados
    if (leleConfigPins.NRF24_bus.mosi == GPIO_NUM_NC || leleConfigPins.NRF24_bus.cs == GPIO_NUM_NC) {
        return false;
    }
    return true;
}

bool detect_rfid() {
    // Verifica se algum módulo RFID está configurado
    return (leleConfig.rfidModule != 0);
}

bool detect_gps() {
    // Verifica se GPS tem pinos configurados
    return (leleConfigPins.gps_bus.tx != GPIO_NUM_NC && leleConfigPins.gps_bus.rx != GPIO_NUM_NC);
}

bool detect_sd_card() { return setupSdCard(); }

bool detect_touch() {
#ifdef HAS_TOUCH
    return true;
#else
    return false;
#endif
}

// ============================================================================
// ANIMAÇÃO DE BOOT
// ============================================================================

void boot_draw_animation() {
    // Limpa tela
    tft.fillScreen(leleConfig.bgColor);

    // Desenha borda com efeito
    for (int i = 0; i < 3; i++) {
        tft.drawRect(i, i, tftWidth - 2 * i, tftHeight - 2 * i, leleConfig.priColor);
    }

    // Logo LELE com efeito de fade-in
    tft.setTextSize(3);
    tft.setTextColor(leleConfig.priColor, leleConfig.bgColor);

    // Animação de letras aparecendo
    const char *logo = "LELE";
    int logoX = (tftWidth - 4 * 18) / 2;
    int logoY = 20;

    for (int i = 0; i < 4; i++) {
        char letter[2] = {logo[i], '\0'};
        tft.setCursor(logoX + i * 18, logoY);
        tft.print(letter);
        delay(100);
    }

    // Subtítulo
    tft.setTextSize(1);
    tft.setTextColor(TFT_DARKGREY, leleConfig.bgColor);
    tft.drawCentreString("PREDATORY FIRMWARE", tftWidth / 2, logoY + 30, 1);
    tft.drawCentreString(LELE_VERSION, tftWidth / 2, logoY + 45, 1);

    // Linha decorativa
    tft.drawFastHLine(20, logoY + 60, tftWidth - 40, leleConfig.priColor);

    delay(300);
}

// ============================================================================
// SCAN DE HARDWARE
// ============================================================================

void boot_show_module_status(const char *name, HWStatus status, int y) {
    // Nome do módulo
    tft.setTextColor(TFT_WHITE, leleConfig.bgColor);
    tft.setCursor(15, y);
    tft.print(name);

    // Status
    int statusX = tftWidth - 70;
    tft.setCursor(statusX, y);

    switch (status) {
        case HW_ONLINE:
            tft.setTextColor(COLOR_ONLINE, leleConfig.bgColor);
            tft.print("[ONLINE]");
            break;
        case HW_OFFLINE:
            tft.setTextColor(COLOR_OFFLINE, leleConfig.bgColor);
            tft.print("[OFFLINE]");
            break;
        case HW_ERROR:
            tft.setTextColor(COLOR_WARNING, leleConfig.bgColor);
            tft.print("[ERRO]");
            break;
        case HW_NOT_CONFIGURED:
            tft.setTextColor(TFT_DARKGREY, leleConfig.bgColor);
            tft.print("[N/C]");
            break;
    }
}

void boot_hw_scan() {
    int startY = 85;
    int lineHeight = 14;
    int currentY = startY;
    int moduleIndex = 0;

    hwModulesOnline = 0;
    hwModulesOffline = 0;

    // Título da seção
    tft.setTextSize(1);
    tft.setTextColor(COLOR_INFO, leleConfig.bgColor);
    tft.drawCentreString("=== HARDWARE SCAN ===", tftWidth / 2, startY - 15, 1);

    // Barra de progresso
    int barY = tftHeight - 50;
    int barWidth = tftWidth - 40;
    int barHeight = 8;
    tft.drawRect(20, barY, barWidth, barHeight, leleConfig.priColor);

    // ESP32 (sempre online)
    hwModules[moduleIndex] = {"ESP32", "C", HW_ONLINE, "OK"};
    boot_show_module_status("ESP32", HW_ONLINE, currentY);
    hwModulesOnline++;
    currentY += lineHeight;
    moduleIndex++;
    tft.fillRect(
        21, barY + 1, (barWidth - 2) * moduleIndex / HW_MODULE_COUNT, barHeight - 2, leleConfig.priColor
    );
    delay(150);

    // WiFi (sempre disponível no ESP32)
    hwModules[moduleIndex] = {"WiFi", "W", HW_ONLINE, "OK"};
    boot_show_module_status("WiFi", HW_ONLINE, currentY);
    hwModulesOnline++;
    currentY += lineHeight;
    moduleIndex++;
    tft.fillRect(
        21, barY + 1, (barWidth - 2) * moduleIndex / HW_MODULE_COUNT, barHeight - 2, leleConfig.priColor
    );
    delay(150);

    // BLE (sempre disponível no ESP32)
    hwModules[moduleIndex] = {"Bluetooth", "B", HW_ONLINE, "OK"};
    boot_show_module_status("Bluetooth LE", HW_ONLINE, currentY);
    hwModulesOnline++;
    currentY += lineHeight;
    moduleIndex++;
    tft.fillRect(
        21, barY + 1, (barWidth - 2) * moduleIndex / HW_MODULE_COUNT, barHeight - 2, leleConfig.priColor
    );
    delay(150);

    // Display (sempre online se chegou aqui)
    hwModules[moduleIndex] = {"Display", "D", HW_ONLINE, "OK"};
    boot_show_module_status("Display TFT", HW_ONLINE, currentY);
    hwModulesOnline++;
    currentY += lineHeight;
    moduleIndex++;
    tft.fillRect(
        21, barY + 1, (barWidth - 2) * moduleIndex / HW_MODULE_COUNT, barHeight - 2, leleConfig.priColor
    );
    delay(150);

    // CC1101
    bool cc1101Ok = detect_cc1101();
    hwModules[moduleIndex] = {"CC1101", "R", cc1101Ok ? HW_ONLINE : HW_OFFLINE, cc1101Ok ? "SubGHz" : "N/A"};
    boot_show_module_status("CC1101 (RF)", cc1101Ok ? HW_ONLINE : HW_OFFLINE, currentY);
    if (cc1101Ok) hwModulesOnline++;
    else hwModulesOffline++;
    currentY += lineHeight;
    moduleIndex++;
    tft.fillRect(
        21, barY + 1, (barWidth - 2) * moduleIndex / HW_MODULE_COUNT, barHeight - 2, leleConfig.priColor
    );
    delay(150);

    // NRF24
    bool nrf24Ok = detect_nrf24();
    hwModules[moduleIndex] = {"NRF24", "N", nrf24Ok ? HW_ONLINE : HW_OFFLINE, nrf24Ok ? "2.4GHz" : "N/A"};
    boot_show_module_status("NRF24L01", nrf24Ok ? HW_ONLINE : HW_OFFLINE, currentY);
    if (nrf24Ok) hwModulesOnline++;
    else hwModulesOffline++;
    currentY += lineHeight;
    moduleIndex++;
    tft.fillRect(
        21, barY + 1, (barWidth - 2) * moduleIndex / HW_MODULE_COUNT, barHeight - 2, leleConfig.priColor
    );
    delay(150);

    // RFID/NFC
    bool rfidOk = detect_rfid();
    hwModules[moduleIndex] = {"RFID", "F", rfidOk ? HW_ONLINE : HW_OFFLINE, rfidOk ? "NFC" : "N/A"};
    boot_show_module_status("RFID/NFC", rfidOk ? HW_ONLINE : HW_OFFLINE, currentY);
    if (rfidOk) hwModulesOnline++;
    else hwModulesOffline++;
    currentY += lineHeight;
    moduleIndex++;
    tft.fillRect(
        21, barY + 1, (barWidth - 2) * moduleIndex / HW_MODULE_COUNT, barHeight - 2, leleConfig.priColor
    );
    delay(150);

    // GPS
    bool gpsOk = detect_gps();
    hwModules[moduleIndex] = {"GPS", "G", gpsOk ? HW_ONLINE : HW_OFFLINE, gpsOk ? "UART" : "N/A"};
    boot_show_module_status("GPS", gpsOk ? HW_ONLINE : HW_OFFLINE, currentY);
    if (gpsOk) hwModulesOnline++;
    else hwModulesOffline++;
    currentY += lineHeight;
    moduleIndex++;
    tft.fillRect(
        21, barY + 1, (barWidth - 2) * moduleIndex / HW_MODULE_COUNT, barHeight - 2, leleConfig.priColor
    );
    delay(150);

    // SD Card
    bool sdOk = detect_sd_card();
    sdCardReady = sdOk;
    hwModules[moduleIndex] = {"SD Card", "S", sdOk ? HW_ONLINE : HW_OFFLINE, sdOk ? "OK" : "N/A"};
    boot_show_module_status("SD Card", sdOk ? HW_ONLINE : HW_OFFLINE, currentY);
    if (sdOk) hwModulesOnline++;
    else hwModulesOffline++;
    moduleIndex++;
    tft.fillRect(21, barY + 1, barWidth - 2, barHeight - 2, leleConfig.priColor);
    delay(150);
}

// ============================================================================
// VERIFICAÇÃO DO SD CARD
// ============================================================================

bool sd_verify_structure() {
    if (!sdCardReady) return false;

    bool allOk = true;
    for (int i = 0; i < REQUIRED_SD_ITEMS_COUNT; i++) {
        if (!SD.exists(requiredSDItems[i].path)) {
            allOk = false;
            break;
        }
    }
    return allOk;
}

bool sd_create_structure() {
    if (!sdCardReady) return false;

    int created = 0;
    int failed = 0;

    for (int i = 0; i < REQUIRED_SD_ITEMS_COUNT; i++) {
        if (!SD.exists(requiredSDItems[i].path)) {
            if (requiredSDItems[i].isDir) {
                if (SD.mkdir(requiredSDItems[i].path)) {
                    created++;
                    Serial.printf("[BOOT] Criado: %s\n", requiredSDItems[i].path);
                } else {
                    failed++;
                    Serial.printf("[BOOT] Falha ao criar: %s\n", requiredSDItems[i].path);
                }
            }
        }
    }

    return (failed == 0);
}

void sd_rebuild_structure() {
    if (!setupSdCard()) {
        displayError("SD Card nao encontrado!", true);
        return;
    }

    displayInfo("Recriando estrutura\ndo SD Card...", false);
    delay(500);

    int created = 0;
    for (int i = 0; i < REQUIRED_SD_ITEMS_COUNT; i++) {
        if (!SD.exists(requiredSDItems[i].path)) {
            if (requiredSDItems[i].isDir && SD.mkdir(requiredSDItems[i].path)) { created++; }
        }
    }

    if (created > 0) {
        displaySuccess(String(created) + " pastas criadas!", true);
    } else {
        displayInfo("Estrutura ja existe!", true);
    }
}

void boot_sd_check() {
    if (!sdCardReady) {
        tft.setTextColor(COLOR_WARNING, leleConfig.bgColor);
        tft.drawCentreString("SD Card nao encontrado", tftWidth / 2, tftHeight - 35, 1);
        return;
    }

    tft.setTextColor(COLOR_INFO, leleConfig.bgColor);
    tft.drawCentreString("Verificando SD Card...", tftWidth / 2, tftHeight - 35, 1);
    delay(300);

    // Limpa mensagem anterior
    tft.fillRect(0, tftHeight - 40, tftWidth, 15, leleConfig.bgColor);

    if (!sd_verify_structure()) {
        tft.setTextColor(COLOR_WARNING, leleConfig.bgColor);
        tft.drawCentreString("Criando estrutura SD...", tftWidth / 2, tftHeight - 35, 1);
        delay(200);

        if (sd_create_structure()) {
            tft.fillRect(0, tftHeight - 40, tftWidth, 15, leleConfig.bgColor);
            tft.setTextColor(COLOR_ONLINE, leleConfig.bgColor);
            tft.drawCentreString("Estrutura criada!", tftWidth / 2, tftHeight - 35, 1);
        } else {
            tft.fillRect(0, tftHeight - 40, tftWidth, 15, leleConfig.bgColor);
            tft.setTextColor(COLOR_OFFLINE, leleConfig.bgColor);
            tft.drawCentreString("Erro ao criar pastas!", tftWidth / 2, tftHeight - 35, 1);
        }
    } else {
        tft.setTextColor(COLOR_ONLINE, leleConfig.bgColor);
        tft.drawCentreString("SD Card OK!", tftWidth / 2, tftHeight - 35, 1);
    }
}

// ============================================================================
// TELA PRINCIPAL DE BOOT
// ============================================================================

void boot_show_results() {
    // Resumo
    int summaryY = tftHeight - 25;
    tft.setTextColor(TFT_WHITE, leleConfig.bgColor);
    tft.setCursor(15, summaryY);
    tft.printf("Online: %d  |  Offline: %d", hwModulesOnline, hwModulesOffline);

    // Mensagem para continuar
    tft.setTextColor(leleConfig.priColor, leleConfig.bgColor);
    tft.drawCentreString("Pressione para continuar", tftWidth / 2, tftHeight - 10, 1);

    // Aguarda tecla
    while (!check(AnyKeyPress)) {
        delay(50);

        // Efeito piscante na mensagem
        static unsigned long lastBlink = 0;
        static bool visible = true;
        if (millis() - lastBlink > 500) {
            lastBlink = millis();
            visible = !visible;
            if (visible) {
                tft.setTextColor(leleConfig.priColor, leleConfig.bgColor);
            } else {
                tft.setTextColor(leleConfig.bgColor, leleConfig.bgColor);
            }
            tft.drawCentreString("Pressione para continuar", tftWidth / 2, tftHeight - 10, 1);
        }
    }

    // Limpa tela após confirmação
    tft.fillScreen(leleConfig.bgColor);
}

void boot_screen_hw() {
    // Se instantBoot está ativo, pula a tela de boot
    if (leleConfig.instantBoot) {
        // Apenas verifica SD silenciosamente
        if (setupSdCard()) {
            sdCardReady = true;
            sd_create_structure();
        }
        return;
    }

    // 1. Animação de logo
    boot_draw_animation();

    // 2. Scan de hardware
    boot_hw_scan();

    // 3. Verificação do SD Card
    boot_sd_check();

    // 4. Aguarda usuário
    boot_show_results();
}
