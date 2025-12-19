#include "rf_433_jammer.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include "core/display.h"
#include "core/mykeyboard.h"

// 433MHz RF Jammer using CC1101
// Jams garage doors, car remotes, sensors on 433MHz band

void rf_433_jammer() {
    drawMainBorderWithTitle("433MHz Jammer");
    padprintln("");
    padprintln("Flooding 433MHz with noise");
    padprintln("All remote controls will fail");
    padprintln("");
    padprintln("Press Any key to STOP.");

    // Initialize CC1101
    ELECHOUSE_cc1101.Init();
    // ELECHOUSE_cc1101.setCarrierFreq(433);
    ELECHOUSE_cc1101.setMHZ(433.92); // Common frequency, can be adjusted
    ELECHOUSE_cc1101.setDeviation(47.6074219);
    ELECHOUSE_cc1101.setModulation(2); // GFSK
    ELECHOUSE_cc1101.setDRate(50000); // 50kbaud like car keys
    ELECHOUSE_cc1101.setPktFormat(0);
    ELECHOUSE_cc1101.setLengthConfig(0);
    ELECHOUSE_cc1101.setCrc(false); // No CRC, just noise
    // ELECHOUSE_cc1101.setPA(PA_LowPower); // Low power, harder to detect
    // ELECHOUSE_cc1101.SendCV(0x00, 0x00); // Disable TX initially

    while (!check(AnyKeyPress)) {
        // Send 255 bytes of random noise, 100 times per second
        for (int i = 0; i < 100; i++) {
            uint8_t noise[255];
            for (int j = 0; j < 255; j++) {
                noise[j] = esp_random() & 0xFF; // Random noise
            }
            ELECHOUSE_cc1101.SendData(noise, 255);
            delay(10); // 100 times per second
        }
    }

    // Stop jamming
    // ELECHOUSE_cc1101.SendCV(0x00, 0x00);
    displaySuccess("Jamming stopped", true);
}
