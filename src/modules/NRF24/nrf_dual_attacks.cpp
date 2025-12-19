#include "nrf_dual_attacks.h"
#include "nrf_common.h"
#include "core/display.h"
#include "core/mykeyboard.h"

// ============================================================================
// DUAL CANAL JAMMER (Carrier Wave)
// ============================================================================
void nrf_dual_jammer() {
    // Inicializa ambos os radios
    if (!nrf_dual_start()) {
        displayError("NRF24 Dual Setup Failed");
        return;
    }

    int channel1 = 40; // Default Ch 1
    int channel2 = 60; // Default Ch 2
    bool active = false;
    bool redraw = true;
    int selected = 0; // 0=Ch1, 1=Ch2, 2=Start/Stop

    while (!check(EscPress)) {
        if (redraw) {
            drawMainBorder();
            tft.setTextSize(FM);
            tft.setCursor(10, 35);
            tft.println("Dual Carrier Jammer");

            tft.setCursor(10, 70);
            tft.setTextColor(selected == 0 ? TFT_BLACK : TFT_WHITE, selected == 0 ? leleConfig.priColor : leleConfig.bgColor);
            tft.printf("Radio 1 CH: %d ", channel1);

            tft.setCursor(10, 90);
            tft.setTextColor(selected == 1 ? TFT_BLACK : TFT_WHITE, selected == 1 ? leleConfig.priColor : leleConfig.bgColor);
            tft.printf("Radio 2 CH: %d ", channel2);

            tft.setCursor(10, 120);
            tft.setTextColor(selected == 2 ? TFT_BLACK : TFT_WHITE, selected == 2 ? leleConfig.priColor : leleConfig.bgColor);
            tft.printf(active ? "STOP JAMMING" : "START JAMMING");

            tft.setTextColor(TFT_WHITE, leleConfig.bgColor);
            tft.setCursor(10, 150);
            if(active) tft.print("STATUS: JAMMING ACTIVE!");
            else       tft.print("STATUS: IDLE");

            redraw = false;
        }

        if (check(NextPress)) {
            if (active) {
                // Se ativo, muda canais dinamicamente
                if (selected == 0) channel1 = (channel1 + 1) % 125;
                if (selected == 1) channel2 = (channel2 + 1) % 125;

                NRFradio.startConstCarrier(RF24_PA_MAX, channel1);
                NRFradio2.startConstCarrier(RF24_PA_MAX, channel2);
            } else {
                 selected = (selected + 1) % 3;
            }
            redraw = true;
        }

        if (check(PrevPress)) {
             if (active) {
                if (selected == 0) channel1 = (channel1 - 1 + 125) % 125;
                if (selected == 1) channel2 = (channel2 - 1 + 125) % 125;

                NRFradio.startConstCarrier(RF24_PA_MAX, channel1);
                NRFradio2.startConstCarrier(RF24_PA_MAX, channel2);
            } else {
                selected = (selected - 1 + 3) % 3;
            }
            redraw = true;
        }

        if (check(SelPress)) {
            if (selected == 2) {
                active = !active;
                if (active) {
                    NRFradio.setPALevel(RF24_PA_MAX);
                    NRFradio.setDataRate(RF24_2MBPS);
                    NRFradio.startConstCarrier(RF24_PA_MAX, channel1);

                    NRFradio2.setPALevel(RF24_PA_MAX);
                    NRFradio2.setDataRate(RF24_2MBPS);
                    NRFradio2.startConstCarrier(RF24_PA_MAX, channel2);
                } else {
                    NRFradio.stopConstCarrier();
                    NRFradio2.stopConstCarrier();
                }
            } else if (!active) {
                // Edita canal
                int *ptr = (selected == 0) ? &channel1 : &channel2;
                *ptr = (*ptr + 5) % 125;
            }
            redraw = true;
        }

        // Random hop mode se segurar botao? (Futuro)
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    NRFradio.stopConstCarrier();
    NRFradio2.stopConstCarrier();
}

// ============================================================================
// HYPER SPAM (BLE Advertising em 2 canais - ex: 37 e 38)
// ============================================================================
void nrf_hyper_spam() {
     if (!nrf_dual_start()) {
        displayError("NRF24 Dual Setup Failed");
        return;
    }

    // Configura radios para BLE mode (simulado)
    // Nota: RF24 lib nao tem suporte nativo a BLE advertising facil sem hacks
    // Por enquanto, faremos um NOISE generator focado nas frequencias de ADV
    // Canal BLE 37 = 2402 MHz (NRF ch 2)
    // Canal BLE 38 = 2426 MHz (NRF ch 26)
    // Canal BLE 39 = 2480 MHz (NRF ch 80)

    int ch_adv1 = 2;  // BLE 37
    int ch_adv2 = 80; // BLE 39

    tft.fillScreen(leleConfig.bgColor);
    tft.setCursor(10, 50);
    tft.setTextSize(FM);
    tft.println("Hyper BLE Jammer");
    tft.println("Targeting Adv Channels...");
    tft.println("R1: Ch 37 (2402 MHz)");
    tft.println("R2: Ch 39 (2480 MHz)");

    NRFradio.setPALevel(RF24_PA_MAX);
    NRFradio2.setPALevel(RF24_PA_MAX);

    NRFradio.startConstCarrier(RF24_PA_MAX, ch_adv1);
    NRFradio2.startConstCarrier(RF24_PA_MAX, ch_adv2);

    while(!check(EscPress)) {
        // Hopping logic entre canais de adv
        // R1: 37 -> 38 -> 37
        // R2: 39 -> 38 -> 39
        vTaskDelay(100 / portTICK_PERIOD_MS);

        NRFradio.startConstCarrier(RF24_PA_MAX, 26); // Ch 38
        NRFradio2.startConstCarrier(RF24_PA_MAX, 26);
        vTaskDelay(50 / portTICK_PERIOD_MS);

        NRFradio.startConstCarrier(RF24_PA_MAX, ch_adv1);
        NRFradio2.startConstCarrier(RF24_PA_MAX, ch_adv2);
    }

    NRFradio.stopConstCarrier();
    NRFradio2.stopConstCarrier();
}

void nrf_protocol_hunter() {
    displayError("Coming Soon: Hunter");
}

void nrf_mouse_jack_dual() {
    displayError("Coming Soon: MouseJack");
}
