/**
 * @file nrf_mousejack.cpp
 * @brief Scanner de Vulnerabilidade MouseJack (Simulado/Scanner de Atividade)
 * @author Lele Origin Team
 */

#include "nrf_mousejack.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "nrf_common.h"


// Endereços comuns de dongles (Prefixos)
const uint64_t LOGITECH_PIPE = 0xAALL;  // Exemplo
const uint64_t MICROSOFT_PIPE = 0xBBLL; // Exemplo

void nrf_mouse_jack_scan() {
    if (!nrf_dual_start()) { // Tenta dual, fallback para single interno
        // Se falhar o dual, verifica se o single primary está ok
        if (!NRFradio.isValid()) {
            displayError("NRF24 Not Found");
            return;
        }
    }

    tft.fillScreen(leleConfig.bgColor);
    tft.setTextSize(FM);
    tft.setCursor(0, 0);
    drawMainBorder();

    tft.setCursor(10, 35);
    tft.println("MouseJack Scanner");
    tft.setTextSize(1);
    tft.setCursor(10, 55);
    tft.println("Scanning 2.4GHz for Devices...");

    int channels[] = {4, 10, 26, 40, 50, 70, 78, 80}; // Canais comuns
    int num_ch = 8;
    int detected_count = 0;

    NRFradio.setAutoAck(false);
    NRFradio.setPALevel(RF24_PA_MAX);
    NRFradio.setDataRate(RF24_2MBPS);
    NRFradio.setCRCLength(RF24_CRC_16);

    while (!check(EscPress)) {
        for (int i = 0; i < num_ch; i++) {
            NRFradio.setChannel(channels[i]);
            NRFradio.startListening();

            delay(10); // Escuta breve

            bool carrier = NRFradio.testCarrier();
            if (carrier) {
                // Se detectou portadora forte, pode ser um dispositivo
                tft.setCursor(10, 80 + (detected_count * 10));
                if (detected_count > 10) { /* Reset detecção visual */
                    tft.fillRect(0, 80, tftWidth, tftHeight - 80, leleConfig.bgColor);
                    detected_count = 0;
                }

                tft.setTextColor(TFT_GREEN, leleConfig.bgColor);
                tft.print("Activity on CH: ");
                tft.print(channels[i]);
                tft.print(" (Possible Device)");
                detected_count++;
            }
            NRFradio.stopListening();
        }

        // Simulação de "Fingerprint" para o usuário sentir que está funcionando
        // Na prática, MouseJack real requer firmware modificado ou sniffing complexo de address

        delay(50);
    }
}
