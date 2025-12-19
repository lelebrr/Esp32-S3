#include "nrf_common.h"
#include "../../core/mykeyboard.h"

RF24 NRFradio(leleConfigPins.NRF24_bus.io0, leleConfigPins.NRF24_bus.cs);
RF24 NRFradio2(leleConfigPins.NRF24_2_bus.io0, leleConfigPins.NRF24_2_bus.cs); // Radio 2
HardwareSerial NRFSerial = HardwareSerial(2); // Uses UART2 for External NRF's
SPIClass *NRFSPI;
SPIClass *NRFSPI2; // SPI bus for Radio 2

void nrf_info() {
    tft.fillScreen(leleConfig.bgColor);
    tft.setTextSize(FM);
    tft.setTextColor(TFT_RED, leleConfig.bgColor);
    tft.drawCentreString("_Disclaimer_", tftWidth / 2, 10, 1);
    tft.setTextColor(TFT_WHITE, leleConfig.bgColor);
    tft.setTextSize(FP);
    tft.setCursor(15, 33);
    padprintln("These functions were made to be used in a controlled environment for STUDY only.");
    padprintln("");
    padprintln("DO NOT use these functions to harm people or companies, you can go to jail!");
    tft.setTextColor(leleConfig.priColor, leleConfig.bgColor);
    padprintln("");
    padprintln(
        "This device is VERY sensible to noise, so long wires or passing near VCC line can make "
        "things go wrong."
    );
    delay(1000);
    while (!check(AnyKeyPress));
}

bool nrf_start(NRF24_MODE mode) {
    bool result = false;
    if (mode == NRF_MODE_DISABLED) return false;

    if (CHECK_NRF_UART(mode)) {
        if (USBserial.getSerialOutput() == &Serial1) {
            displayError("(E) UART already in use", true);
            return false;
        }
        NRFSerial.begin(115200, SERIAL_8N1, leleConfigPins.uart_bus.rx, leleConfigPins.uart_bus.tx);
        Serial.println("NRF24 on Serial Started");
        result = true;
    };

    if (!CHECK_NRF_SPI(mode)) return result;
    pinMode(leleConfigPins.NRF24_bus.cs, OUTPUT);
    digitalWrite(leleConfigPins.NRF24_bus.cs, HIGH);
    pinMode(leleConfigPins.NRF24_bus.io0, OUTPUT);
    digitalWrite(leleConfigPins.NRF24_bus.io0, LOW);

    if (leleConfigPins.NRF24_bus.mosi == (gpio_num_t)TFT_MOSI &&
        leleConfigPins.NRF24_bus.mosi != GPIO_NUM_NC) { // (T_EMBED), CORE2 and others
#if TFT_MOSI > 0 // condition for Headless and 8bit displays (no SPI bus)
        NRFSPI = &tft.getSPIinstance();
#else
        NRFSPI = &SPI;
#endif

    } else if (leleConfigPins.NRF24_bus.mosi == leleConfigPins.SDCARD_bus.mosi) {
        // CC1101 shares SPI with SDCard (Cardputer and CYDs)

        NRFSPI = &sdcardSPI;
    } else if (leleConfigPins.NRF24_bus.mosi == leleConfigPins.CC1101_bus.mosi &&
               leleConfigPins.NRF24_bus.mosi != leleConfigPins.SDCARD_bus.mosi) {
        // Smoochie board shares CC1101 and NRF24 SPI bus with different CS pins at
        // the same time, different from StickCs that uses the same Bus, but one at a
        // time (same CS Pin)
        NRFSPI = &CC_NRF_SPI;
    } else {
        NRFSPI = &SPI;
    }
    NRFSPI->begin(
        (int8_t)leleConfigPins.NRF24_bus.sck,
        (int8_t)leleConfigPins.NRF24_bus.miso,
        (int8_t)leleConfigPins.NRF24_bus.mosi
    );
    delay(10);

    if (NRFradio.begin(
            NRFSPI,
            rf24_gpio_pin_t(leleConfigPins.NRF24_bus.io0),
            rf24_gpio_pin_t(leleConfigPins.NRF24_bus.cs)
        )) {
        result = true;
    } else {
        return false;
    }
    return result;
}

// Inicializa Dual Radio (apenas SPI)
bool nrf_dual_start() {
    bool radio1_ok = nrf_start(NRF_MODE_SPI);

    // Configura Radio 2 (HSPI geralmente, bus da tela)
    // Se o bus 2 for o mesmo da tela, usamos o SPI global ou um novo
    // Para simplificar, assumimos um novo HSPI se pinos forem diferentes

    pinMode(leleConfigPins.NRF24_2_bus.cs, OUTPUT);
    digitalWrite(leleConfigPins.NRF24_2_bus.cs, HIGH);
    pinMode(leleConfigPins.NRF24_2_bus.io0, OUTPUT);
    digitalWrite(leleConfigPins.NRF24_2_bus.io0, LOW);

    // Usa HSPI (SPI2) normalmente se disponível, ou SPI custom
    // Como CYD usa VSPI para NRF1/SD/Touch, usaremos HSPI para NRF2
    static SPIClass HSPI_BUS(HSPI);
    NRFSPI2 = &HSPI_BUS;

    NRFSPI2->begin(
        (int8_t)leleConfigPins.NRF24_2_bus.sck,
        (int8_t)leleConfigPins.NRF24_2_bus.miso,
        (int8_t)leleConfigPins.NRF24_2_bus.mosi
    );

    bool radio2_ok = NRFradio2.begin(
        NRFSPI2,
        rf24_gpio_pin_t(leleConfigPins.NRF24_2_bus.io0), // CE
        rf24_gpio_pin_t(leleConfigPins.NRF24_2_bus.cs)   // CS
    );

    if(radio2_ok) {
        Serial.println("NRF24 Radio 2 Init OK");
    } else {
        Serial.println("NRF24 Radio 2 Init FAILED");
    }

    return radio1_ok || radio2_ok; // Retorna true se pelo menos um funcionar
}

NRF24_MODE nrf_setMode() {
    NRF24_MODE mode = NRF_MODE_DISABLED;
    options = {
        {"SPI Mode",  [&]() { mode = NRF_MODE_SPI; } },
        {"SPI UART",  [&]() { mode = NRF_MODE_UART; }},
        {"SPI BOTH",  [&]() { mode = NRF_MODE_BOTH; }},
        {"Menu Principal", [=]() { returnToMenu = true; } }
    };
    loopOptions(options);
    return mode;
}
