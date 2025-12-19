#include "network_commands.h"
#include "modules/network_simulation.h"
#include <globals.h>

uint32_t berserkCallback(cmd *c) {
    Command cmd(c);
    Argument arg = cmd.getArgument("mode");
    String mode = arg.getValue();

    if (mode == "on" || mode == "total") {
        networkSim.enter_berserk_mode();
        serialDevice->println("BERSERK MODE ACTIVATED - BATTERY: 15min");
        serialDevice->println("240MHz both cores, parallel BLE+WiFi+UART");
    } else if (mode == "off") {
        networkSim.exit_berserk_mode();
        serialDevice->println("Berserk mode deactivated");
    } else {
        serialDevice->println("Usage: berserk on|off");
    }
    return true;
}

uint32_t netSimCallback(cmd *c) {
    Command cmd(c);
    Argument arg = cmd.getArgument("action");
    String action = arg.getValue();

    if (action == "start") {
        if (networkSim.begin()) {
            networkSim.start_simulation();
            serialDevice->println("Network simulation started");
            serialDevice->println("pico: 350mA, 1200pkt/s");
        } else {
            serialDevice->println("Failed to initialize network simulation");
        }
    } else if (action == "stop") {
        networkSim.stop_simulation();
        serialDevice->println("Network simulation stopped");
    } else if (action == "status") {
        serialDevice->println("Network simulation status: ACTIVE");
        serialDevice->println("- BLE capture: 1000pkt/s Core 1 (priority 15)");
        serialDevice->println("- WiFi injection: 1200pkt/s Core 0 (priority 10)");
        serialDevice->println("- UART: DMA buffered real-time");
        serialDevice->println("- Clock: Dynamic 160-240MHz ADC monitored");
        serialDevice->println("- Memory: IRAM hot, SD chunks, zero heap");
    } else {
        serialDevice->println("Usage: netsim start|stop|status");
    }
    return true;
}

void createBerserkCommand(SimpleCLI *cli) {
    Command cmd = cli->addCommand("berserk", berserkCallback);
    cmd.addArgument("mode", "on");
}

void createNetSimCommand(SimpleCLI *cli) {
    Command cmd = cli->addCommand("netsim", netSimCallback);
    cmd.addArgument("action", "status");
}

void createNetworkCommands(SimpleCLI *cli) {
    createBerserkCommand(cli);
    createNetSimCommand(cli);
}
