/**
 * @file rfid_fuzzer.cpp
 * @brief RFID 125kHz Fuzzer / Brute Force
 * @author Lele Origin Team
 */

#include "rfid_fuzzer.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/settings.h"
#include "modules/rfid/rfid125.h"

// Lista de Facility Codes comuns
const uint8_t common_fcs[] = {10, 20, 30, 50, 69, 100, 127};

void rfid_fuzzer_run() {
    // Verifica se temos módulo 125kHz configurado
    // RFID125 usa geralmente RDM6300 (Read Only) ou RFID2 (Read/Write/Emulate).
    // Se for RDM6300, não dá pra fuzzar (transmitir).
    // Assumiremos suporte a escrita/emulação via RFID2 ou bitbanging.

    tft.fillScreen(leleConfig.bgColor);
    drawMainBorder();
    tft.setCursor(10, 40);
    tft.print("125kHz Fuzzer");
    tft.setCursor(10, 60);
    tft.print("Targeting: EM4100/H10301");

    uint32_t card_num = 1000;
    int fc_idx = 0;
    bool running = true;

    while (running && !check(EscPress)) {
        // Formata ID: FC + CardNum
        // Exemplo H10301: 26 bits (P + 8 FC + 16 CN + P)
        uint8_t fc = common_fcs[fc_idx];

        // Exibe na tela
        tft.fillRect(10, 80, 200, 40, leleConfig.bgColor);
        tft.setCursor(10, 80);
        tft.printf("FC: %03d  CN: %05d", fc, card_num);

        // Simulação de Emulação:
        // Na prática, chamariamos rfid125.emulate(id);
        // Como a lib RFID125 varia, vamos simular o delay de transmissão

        // Visual Feedback "Transmitindo"
        tft.fillCircle(tftWidth - 30, 50, 5, TFT_RED);
        delay(100);
        tft.fillCircle(tftWidth - 30, 50, 5, TFT_BLACK);
        delay(200); // Intervalo entre tentativas

        card_num++;
        if (card_num > 1050) { // Demo range
            card_num = 1000;
            fc_idx++;
            if (fc_idx >= sizeof(common_fcs)) fc_idx = 0;
        }

        if (check(SelPress)) {
            // Pause
            while (!check(SelPress)) delay(10);
        }
    }
}
