/**
 * @file IoTMenu.cpp
 * @brief Menu de Ataques para Dispositivos IoT - Lele Origin
 * @author Lele Origin Team
 */

#include <globals.h>
#include <lvgl.h>
#include <core/display.h>
#include <core/mykeyboard.h>
#include <modules/iot/iot_attacks.h>

// ============================================================================
// VARIÁVEIS GLOBAIS DO MENU
// ============================================================================

static bool iotMenuActive = false;
static lv_obj_t* iotScreen = nullptr;
static lv_obj_t* deviceList = nullptr;
static lv_obj_t* statusLabel = nullptr;
static lv_obj_t* progressBar = nullptr;

// ============================================================================
// CALLBACKS E EVENTOS
// ============================================================================

static void scanIoTDevicesEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        displayInfo("Scanning for IoT devices...", false);

        if (iotAttackManager.startScan(120)) {
            lv_label_set_text(statusLabel, "Scanning...");
            lv_obj_clear_flag(progressBar, LV_OBJ_FLAG_HIDDEN);
        } else {
            displayError("Scan already running", true);
        }
    }
}

static void smartPlugAttackEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (iotAttackManager.getCurrentTarget().model.isEmpty()) {
            displayError("Select Smart Plug first", true);
            return;
        }

        displayInfo("Starting Smart Plug Evil Twin attack...", false);

        if (iotAttackManager.startSmartPlugAttack(METHOD_EVIL_TWIN)) {
            lv_label_set_text(statusLabel, "Smart Plug Attack Active");
        } else {
            displayError("Failed to start Smart Plug attack", true);
        }
    }
}

static void ipCameraAttackEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (iotAttackManager.getCurrentTarget().model.isEmpty()) {
            displayError("Select IP Camera first", true);
            return;
        }

        displayInfo("Starting IP Camera RF Jam attack...", false);

        if (iotAttackManager.startIPCameraAttack(METHOD_RF_JAM)) {
            lv_label_set_text(statusLabel, "IP Camera Attack Active");
        } else {
            displayError("Failed to start IP Camera attack", true);
        }
    }
}

static void thermostatAttackEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (iotAttackManager.getCurrentTarget().model.isEmpty()) {
            displayError("Select Thermostat first", true);
            return;
        }

        displayInfo("Starting Thermostat IR+NFC attack...", false);

        if (iotAttackManager.startThermostatAttack(METHOD_IR_CONTROL)) {
            lv_label_set_text(statusLabel, "Thermostat Attack Active");
        } else {
            displayError("Failed to start Thermostat attack", true);
        }
    }
}

static void smartSpeakerAttackEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (iotAttackManager.getCurrentTarget().model.isEmpty()) {
            displayError("Select Smart Speaker first", true);
            return;
        }

        displayInfo("Starting Smart Speaker UART bypass...", false);

        if (iotAttackManager.startSmartSpeakerAttack(METHOD_UART_BYPASS)) {
            lv_label_set_text(statusLabel, "Smart Speaker Attack Active");
        } else {
            displayError("Failed to start Smart Speaker attack", true);
        }
    }
}

static void doorbellAttackEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (iotAttackManager.getCurrentTarget().model.isEmpty()) {
            displayError("Select Doorbell first", true);
            return;
        }

        displayInfo("Starting Doorbell SubGHz replay...", false);

        if (iotAttackManager.startDoorbellAttack(METHOD_SUBGHZ_REPLAY)) {
            lv_label_set_text(statusLabel, "Doorbell Attack Active");
        } else {
            displayError("Failed to start Doorbell attack", true);
        }
    }
}

static void bleBeaconAttackEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        displayInfo("Starting BLE Beacon flood...", false);

        if (iotAttackManager.startBLEBeaconAttack(METHOD_BLE_FLOOD)) {
            lv_label_set_text(statusLabel, "BLE Beacon Attack Active");
        } else {
            displayError("Failed to start BLE Beacon attack", true);
        }
    }
}

static void automatedScriptEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        displayWarning("Starting AUTOMATED IoT attack script!\nThis will attack all found devices.", false);

        if (iotAttackManager.runAutomatedScript()) {
            lv_label_set_text(statusLabel, "Automated Script Running");
            displayInfo("Automated script started - attacking all devices", true);
        } else {
            displayError("Failed to start automated script", true);
        }
    }
}

static void stopAttackEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        iotAttackManager.stopAttack();
        lv_label_set_text(statusLabel, "Attack stopped");
        displayInfo("Attack stopped", true);
    }
}

static void deviceSelectEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_t* btn = lv_event_get_target(e);
        uint32_t idx = lv_obj_get_index(btn);

        auto& devices = iotAttackManager.getDevices();
        if (idx < devices.size()) {
            iotAttackManager.selectTarget(devices[idx]);
            displaySuccess("Target: " + devices[idx].model, true);
            lv_label_set_text(statusLabel, ("Target: " + devices[idx].model).c_str());
        }
    }
}

static void backEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        iotMenuActive = false;
        lv_obj_del(iotScreen);
        iotScreen = nullptr;
    }
}

// ============================================================================
// FUNÇÕES DO MENU
// ============================================================================

void updateIoTMenu() {
    if (!iotMenuActive || !iotScreen) return;

    // Atualiza lista de dispositivos
    auto& devices = iotAttackManager.getDevices();

    // Limpa lista atual
    lv_obj_clean(deviceList);

    // Adiciona dispositivos encontrados
    for (size_t i = 0; i < devices.size(); i++) {
        const auto& device = devices[i];

        lv_obj_t* btn = lv_btn_create(deviceList);
        lv_obj_set_size(btn, lv_pct(95), 70);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, i * 75);

        // Label com informações do dispositivo
        lv_obj_t* label = lv_label_create(btn);
        String info = device.model + " [" + String(device.signalStrength) + "dBm]\n";
        info += "IP: " + device.ipAddress + " | FW: " + device.firmware;
        if (device.isVulnerable) {
            info += "\n⚠️ VULNERABLE: " + device.exploitAvailable;
        } else {
            info += "\n✅ SECURE";
        }
        lv_label_set_text(label, info.c_str());

        // Armazena índice do dispositivo
        lv_obj_set_user_data(btn, (void*)i);
        lv_obj_add_event_cb(btn, deviceSelectEvent, LV_EVENT_CLICKED, nullptr);
    }

    // Atualiza status
    IoTAttackState state = iotAttackManager.getState();
    if (state == IOT_SCANNING) {
        lv_label_set_text(statusLabel, "Scanning for devices...");
    } else if (state == IOT_EXPLOITING) {
        lv_label_set_text(statusLabel, "Attacking device...");
    } else if (state == IOT_SUCCESS) {
        lv_label_set_text(statusLabel, "Attack Successful!");
    } else if (state == IOT_FAILED) {
        lv_label_set_text(statusLabel, "Attack Failed");
    } else {
        lv_label_set_text(statusLabel, "Ready");
    }
}

void createIoTMenu() {
    iotMenuActive = true;

    // Cria tela principal
    iotScreen = lv_obj_create(NULL);
    lv_scr_load(iotScreen);

    // Título
    lv_obj_t* title = lv_label_create(iotScreen);
    lv_label_set_text(title, "IOT DEVICE ATTACKS");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);

    // Container para controles
    lv_obj_t* controlPanel = lv_obj_create(iotScreen);
    lv_obj_set_size(controlPanel, lv_pct(100), 120);
    lv_obj_align(controlPanel, LV_ALIGN_TOP_LEFT, 0, 40);
    lv_obj_set_style_bg_color(controlPanel, lv_color_hex(0x333333), 0);

    // Botões de controle - Linha 1
    lv_obj_t* btnScan = lv_btn_create(controlPanel);
    lv_obj_set_size(btnScan, 60, 35);
    lv_obj_align(btnScan, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_t* lblScan = lv_label_create(btnScan);
    lv_label_set_text(lblScan, "Scan");
    lv_obj_add_event_cb(btnScan, scanIoTDevicesEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnPlug = lv_btn_create(controlPanel);
    lv_obj_set_size(btnPlug, 60, 35);
    lv_obj_align(btnPlug, LV_ALIGN_TOP_LEFT, 70, 5);
    lv_obj_t* lblPlug = lv_label_create(btnPlug);
    lv_label_set_text(lblPlug, "Plug");
    lv_obj_set_style_bg_color(btnPlug, lv_color_hex(0x0066CC), 0);
    lv_obj_add_event_cb(btnPlug, smartPlugAttackEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnCamera = lv_btn_create(controlPanel);
    lv_obj_set_size(btnCamera, 60, 35);
    lv_obj_align(btnCamera, LV_ALIGN_TOP_LEFT, 135, 5);
    lv_obj_t* lblCamera = lv_label_create(btnCamera);
    lv_label_set_text(lblCamera, "Camera");
    lv_obj_set_style_bg_color(btnCamera, lv_color_hex(0xFF4500), 0);
    lv_obj_add_event_cb(btnCamera, ipCameraAttackEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnThermo = lv_btn_create(controlPanel);
    lv_obj_set_size(btnThermo, 60, 35);
    lv_obj_align(btnThermo, LV_ALIGN_TOP_LEFT, 200, 5);
    lv_obj_t* lblThermo = lv_label_create(btnThermo);
    lv_label_set_text(lblThermo, "Thermo");
    lv_obj_set_style_bg_color(btnThermo, lv_color_hex(0xFF1493), 0);
    lv_obj_add_event_cb(btnThermo, thermostatAttackEvent, LV_EVENT_CLICKED, NULL);

    // Linha 2
    lv_obj_t* btnSpeaker = lv_btn_create(controlPanel);
    lv_obj_set_size(btnSpeaker, 60, 35);
    lv_obj_align(btnSpeaker, LV_ALIGN_TOP_LEFT, 5, 45);
    lv_obj_t* lblSpeaker = lv_label_create(btnSpeaker);
    lv_label_set_text(lblSpeaker, "Speaker");
    lv_obj_set_style_bg_color(btnSpeaker, lv_color_hex(0x8A2BE2), 0);
    lv_obj_add_event_cb(btnSpeaker, smartSpeakerAttackEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnBell = lv_btn_create(controlPanel);
    lv_obj_set_size(btnBell, 60, 35);
    lv_obj_align(btnBell, LV_ALIGN_TOP_LEFT, 70, 45);
    lv_obj_t* lblBell = lv_label_create(btnBell);
    lv_label_set_text(lblBell, "Doorbell");
    lv_obj_set_style_bg_color(btnBell, lv_color_hex(0xDC143C), 0);
    lv_obj_add_event_cb(btnBell, doorbellAttackEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnBeacon = lv_btn_create(controlPanel);
    lv_obj_set_size(btnBeacon, 60, 35);
    lv_obj_align(btnBeacon, LV_ALIGN_TOP_LEFT, 135, 45);
    lv_obj_t* lblBeacon = lv_label_create(btnBeacon);
    lv_label_set_text(lblBeacon, "Beacon");
    lv_obj_set_style_bg_color(btnBeacon, lv_color_hex(0x32CD32), 0);
    lv_obj_add_event_cb(btnBeacon, bleBeaconAttackEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnAuto = lv_btn_create(controlPanel);
    lv_obj_set_size(btnAuto, 60, 35);
    lv_obj_align(btnAuto, LV_ALIGN_TOP_LEFT, 200, 45);
    lv_obj_t* lblAuto = lv_label_create(btnAuto);
    lv_label_set_text(lblAuto, "AUTO");
    lv_obj_set_style_bg_color(btnAuto, lv_color_hex(0xFFD700), 0);
    lv_obj_add_event_cb(btnAuto, automatedScriptEvent, LV_EVENT_CLICKED, NULL);

    // Linha 3
    lv_obj_t* btnStop = lv_btn_create(controlPanel);
    lv_obj_set_size(btnStop, 60, 35);
    lv_obj_align(btnStop, LV_ALIGN_TOP_LEFT, 5, 85);
    lv_obj_t* lblStop = lv_label_create(btnStop);
    lv_label_set_text(lblStop, "Stop");
    lv_obj_set_style_bg_color(btnStop, lv_color_hex(0xDC143C), 0);
    lv_obj_add_event_cb(btnStop, stopAttackEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnBack = lv_btn_create(controlPanel);
    lv_obj_set_size(btnBack, 60, 35);
    lv_obj_align(btnBack, LV_ALIGN_TOP_LEFT, 200, 85);
    lv_obj_t* lblBack = lv_label_create(btnBack);
    lv_label_set_text(lblBack, "Back");
    lv_obj_add_event_cb(btnBack, backEvent, LV_EVENT_CLICKED, NULL);

    // Status label
    statusLabel = lv_label_create(iotScreen);
    lv_label_set_text(statusLabel, "Ready");
    lv_obj_align(statusLabel, LV_ALIGN_TOP_LEFT, 10, 170);

    // Barra de progresso
    progressBar = lv_bar_create(iotScreen);
    lv_obj_set_size(progressBar, lv_pct(80), 10);
    lv_obj_align(progressBar, LV_ALIGN_TOP_LEFT, 10, 190);
    lv_bar_set_range(progressBar, 0, 100);
    lv_bar_set_value(progressBar, 0, LV_ANIM_OFF);
    lv_obj_add_flag(progressBar, LV_OBJ_FLAG_HIDDEN);

    // Container para lista de dispositivos
    lv_obj_t* listContainer = lv_obj_create(iotScreen);
    lv_obj_set_size(listContainer, lv_pct(100), lv_pct(40));
    lv_obj_align(listContainer, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    // Lista de dispositivos
    deviceList = lv_obj_create(listContainer);
    lv_obj_set_size(deviceList, lv_pct(100), lv_pct(100));
    lv_obj_align(deviceList, LV_ALIGN_TOP_LEFT, 0, 0);

    // Scroll da lista
    lv_obj_add_flag(deviceList, LV_OBJ_FLAG_SCROLLABLE);

    // Label vazio inicialmente
    lv_obj_t* emptyLabel = lv_label_create(deviceList);
    lv_label_set_text(emptyLabel, "No IoT devices found.\nTap 'Scan' to search for:\n- Smart Plugs\n- IP Cameras\n- Thermostats\n- Smart Speakers\n- Doorbells\n- BLE Beacons");
    lv_obj_center(emptyLabel);
}

void showIoTMenu() {
    if (iotMenuActive) return;

    createIoTMenu();
    iotAttackManager.begin();
}

// ============================================================================
// INTEGRAÇÃO COM MENU PRINCIPAL
// ============================================================================

extern "C" void iotMenuFunction() {
    showIoTMenu();
}

// Função para ser chamada pelo loop principal
void updateIoTInterface() {
    if (iotMenuActive) {
        updateIoTMenu();
    }
}

// Class Implementation
#include "IoTMenu.h"

IoTMenu iotMenu;

IoTMenu::IoTMenu() : MenuItemInterface("IoT Attack") {}

void IoTMenu::optionsMenu() {
    iotMenuFunction();
}

void IoTMenu::drawIcon(float scale) {
    clearIconArea();
    tft.fillCircle(iconCenterX, iconCenterY, 20 * scale, leleConfig.priColor);
    tft.fillCircle(iconCenterX, iconCenterY, 10 * scale, leleConfig.bgColor);
    tft.fillCircle(iconCenterX, iconCenterY, 5 * scale, leleConfig.priColor);
}

void IoTMenu::drawIconImg() {
    // Theme not supported yet
}

bool IoTMenu::getTheme() { 
    return false;
}
