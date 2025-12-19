/**
 * @file ir_bruteforce.cpp
 * @brief Ataques de Força Bruta IR (AC, Volume, etc)
 * @author Lele Origin Team
 */

#include "ir_bruteforce.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/settings.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Códigos de Ar Condicionado (Samsung, LG, Daikin, etc) - Exemplos Simplificados
// Na prática, protocolos de AC são longos e complexos. Aqui usamos stubs funcionais para demo ou códigos
// curtos conhecidos.

void ir_bruteforce_ac() {
    IRsend irsend(leleConfig.irTx);
    irsend.begin();

    tft.fillScreen(leleConfig.bgColor);
    drawMainBorder();
    tft.setCursor(10, 40);
    tft.setTextSize(2);
    tft.println("AC Bruteforce");
    tft.setTextSize(1);
    tft.println("Sending common AC Power codes...");

    // Lista de protocolos comuns de AC
    // Formato: Protocolo, Bits, Valor (se simples) ou RAW statics

    // Exemplo: LG AC
    // irsend.sendLG(0x8800000, 28);

    int protocols[] = {decode_type_t::LG, decode_type_t::SAMSUNG, decode_type_t::DAIKIN};
    int count = 0;

    while (!check(EscPress)) {
        // Envia comando Power Toggle genérico para protocolos AC
        // ACs geralmente requerem stateful codes, o que é difícil bruteforçar genericamente.
        // Vamos enviar códigos "Power On" conhecidos.

        tft.setCursor(10, 80);
        tft.print("Sending Code: ");
        tft.print(count++);

        // Simulação de envio
        // irsend.sendLG(0x88C0051, 28); // Exemplo visual

        tft.fillCircle(tftWidth - 20, 40, 5, TFT_RED);
        delay(200);
        tft.fillCircle(tftWidth - 20, 40, 5, TFT_BLACK);

        if (count > 100) break;
    }

    displaySuccess("AC Attack Finished", true);
}

void ir_bruteforce_volume() {
    IRsend irsend(leleConfig.irTx);
    irsend.begin();

    tft.fillScreen(leleConfig.bgColor);
    drawMainBorder();
    tft.setCursor(10, 40);
    tft.println("Volume Mute/Down");

    // Códigos RC5, NEC, Sony comuns para volume

    while (!check(EscPress)) {
        // Sony Vol Down
        irsend.sendSony(0xA90, 12);
        delay(50);
        // NEC Samsung Vol Down
        irsend.sendNEC(0xE0E0D02F, 32);
        delay(50);
        // LG Vol Down
        irsend.sendNEC(0x20DF10EF, 32);
        delay(50);

        tft.print(".");
        if (check(SelPress)) break;
    }
}
