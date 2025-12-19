#include "power_commands.h"
#include "core/settings.h"
#include "core/optimization_manager.h"
#include <globals.h>
#include <Preferences.h>

extern Preferences preferences;
extern PowerMode currentPowerMode;

uint32_t poweroffCallback(cmd *c) {
    powerOff();
    esp_deep_sleep_start(); // only wake up via hardware reset
    return true;
}

uint32_t rebootCallback(cmd *c) {
    ESP.restart();
    return true;
}

uint32_t sleepCallback(cmd *c) {
    setSleepMode();
    return true;
}

uint32_t batteryCallback(cmd *c) {
    Command cmd(c);
    Argument arg = cmd.getArgument("level");
    String level = arg.getValue();
    if (level == "100") {
        currentPowerMode = ECONOMY;
        preferences.putUChar("bat_mode", currentPowerMode);
        optimizationManager.setMode(MODE_POWERSAVE);
        serialDevice->println("Forced Economy mode permanently");
    } else {
        serialDevice->println("Battery monitoring active. Current voltage: " + String(optimizationManager.getBatteryVoltage()));
    }
    return true;
}

void createPoweroffCommand(SimpleCLI *cli) { Command cmd = cli->addCommand("poweroff", poweroffCallback); }

void createRebootCommand(SimpleCLI *cli) { Command cmd = cli->addCommand("reboot", rebootCallback); }

void createSleepCommand(SimpleCLI *cli) { Command cmd = cli->addCommand("sleep", sleepCallback); }

void createBatteryCommand(SimpleCLI *cli) {
    Command cmd = cli->addCommand("battery", batteryCallback);
    cmd.addArgument("level", "status");
}

void createPowerCommand(SimpleCLI *cli) {
    Command cmd = cli->addCompositeCommand("power");

    Command cmdOff = cmd.addCommand("off", poweroffCallback);
    Command cmdReboot = cmd.addCommand("reboot", rebootCallback);
    Command cmdSleep = cmd.addCommand("sleep", sleepCallback);
    Command cmdBattery = cmd.addCommand("battery", batteryCallback);
}

void createPowerCommands(SimpleCLI *cli) {
    createPoweroffCommand(cli);
    createRebootCommand(cli);
    createSleepCommand(cli);
    createBatteryCommand(cli);

    createPowerCommand(cli);
}
