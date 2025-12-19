/**
 * @file ConsoleMenu.cpp
 * @brief Menu de Ataques para Consoles Antigos - Lele Origin
 * @author Lele Origin Team
 */

#include <globals.h>
#include <lvgl.h>
#include <core/display.h>
#include <core/mykeyboard.h>
#include <modules/consoles/console_attacks.h>

// ============================================================================
// VARIÁVEIS GLOBAIS DO MENU
// ============================================================================

static bool consoleMenuActive = false;
static lv_obj_t* consoleScreen = nullptr;
static lv_obj_t* consoleList = nullptr;
static lv_obj_t* statusLabel = nullptr;
static lv_obj_t* progressBar = nullptr;

// ============================================================================
// CALLBACKS E EVENTOS
// ============================================================================

static void scanConsolesEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        displayInfo("Scanning for consoles...", false);

        if (consoleAttackManager.startScan(60)) {
            lv_label_set_text(statusLabel, "Scanning...");
            lv_obj_clear_flag(progressBar, LV_OBJ_FLAG_HIDDEN);
        } else {
            displayError("Scan already running", true);
        }
    }
}

static void ps3AttackEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (consoleAttackManager.getCurrentTarget().model.isEmpty()) {
            displayError("Select PS3 first", true);
            return;
        }

        displayInfo("Starting PS3 BadUSB attack...", false);

        if (consoleAttackManager.startPS3Attack(METHOD_BADUSB)) {
            lv_label_set_text(statusLabel, "PS3 Attack Active");
        } else {
            displayError("Failed to start PS3 attack", true);
        }
    }
}

static void xbox360AttackEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (consoleAttackManager.getCurrentTarget().model.isEmpty()) {
            displayError("Select Xbox 360 first", true);
            return;
        }

        displayInfo("Starting Xbox 360 JTAG attack...", false);

        if (consoleAttackManager.startXbox360Attack(METHOD_JTAG)) {
            lv_label_set_text(statusLabel, "Xbox 360 Attack Active");
        } else {
            displayError("Failed to start Xbox 360 attack", true);
        }
    }
}

static void wiiuAttackEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (consoleAttackManager.getCurrentTarget().model.isEmpty()) {
            displayError("Select Wii U first", true);
            return;
        }

        displayInfo("Starting Wii U Evil Twin attack...", false);

        if (consoleAttackManager.startWiiUAttack(METHOD_EVIL_TWIN)) {
            lv_label_set_text(statusLabel, "Wii U Attack Active");
        } else {
            displayError("Failed to start Wii U attack", true);
        }
    }
}

static void switchAttackEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (consoleAttackManager.getCurrentTarget().model.isEmpty()) {
            displayError("Select Switch first", true);
            return;
        }

        displayInfo("Starting Switch IR glitching...", false);

        if (consoleAttackManager.startSwitchAttack(METHOD_GLITCHING)) {
            lv_label_set_text(statusLabel, "Switch Attack Active");
        } else {
            displayError("Failed to start Switch attack", true);
        }
    }
}

static void micBleedAttackEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        String command = keyboard("open_settings", 200, "Voice Command");

        displayInfo("Starting microphone bleed attack...", false);

        if (consoleAttackManager.startMicBleedAttack(command)) {
            lv_label_set_text(statusLabel, "Mic Bleed Active");
        } else {
            displayError("Failed to start mic bleed", true);
        }
    }
}

static void stopAttackEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        consoleAttackManager.stopAttack();
        lv_label_set_text(statusLabel, "Attack stopped");
        displayInfo("Attack stopped", true);
    }
}

static void consoleSelectEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_t* btn = lv_event_get_target(e);
        uint32_t idx = lv_obj_get_index(btn);

        auto& consoles = consoleAttackManager.getConsoles();
        if (idx < consoles.size()) {
            consoleAttackManager.selectTarget(consoles[idx]);
            displaySuccess("Target: " + consoles[idx].model, true);
            lv_label_set_text(statusLabel, ("Target: " + consoles[idx].model).c_str());
        }
    }
}

static void backEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        consoleMenuActive = false;
        lv_obj_del(consoleScreen);
        consoleScreen = nullptr;
    }
}

// ============================================================================
// FUNÇÕES DO MENU
// ============================================================================

void updateConsoleMenu() {
    if (!consoleMenuActive || !consoleScreen) return;

    // Atualiza lista de consoles
    auto& consoles = consoleAttackManager.getConsoles();

    // Limpa lista atual
    lv_obj_clean(consoleList);

    // Adiciona consoles encontrados
    for (size_t i = 0; i < consoles.size(); i++) {
        const auto& console = consoles[i];

        lv_obj_t* btn = lv_btn_create(consoleList);
        lv_obj_set_size(btn, lv_pct(95), 60);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, i * 65);

        // Label com informações do console
        lv_obj_t* label = lv_label_create(btn);
        String info = console.model + " [" + String(console.signalStrength) + "dBm]\n";
        info += "FW: " + console.firmware;
        if (console.isVulnerable) {
            info += " ⚠️ VULNERABLE";
        }
        lv_label_set_text(label, info.c_str());

        // Armazena índice do console
        lv_obj_set_user_data(btn, (void*)i);
        lv_obj_add_event_cb(btn, consoleSelectEvent, LV_EVENT_CLICKED, nullptr);
    }

    // Atualiza barra de progresso se estiver escaneando
    if (consoleAttackManager.getState() == CONSOLE_SCANNING) {
        lv_label_set_text(statusLabel, "Scanning...");
    } else if (consoleAttackManager.getState() == CONSOLE_EXPLOITING) {
        lv_label_set_text(statusLabel, "Attacking...");
    } else if (consoleAttackManager.getState() == CONSOLE_SUCCESS) {
        lv_label_set_text(statusLabel, "Attack Successful!");
    } else if (consoleAttackManager.getState() == CONSOLE_FAILED) {
        lv_label_set_text(statusLabel, "Attack Failed");
    } else {
        lv_label_set_text(statusLabel, "Ready");
    }
}

void createConsoleMenu() {
    consoleMenuActive = true;

    // Cria tela principal
    consoleScreen = lv_obj_create(NULL);
    lv_scr_load(consoleScreen);

    // Título
    lv_obj_t* title = lv_label_create(consoleScreen);
    lv_label_set_text(title, "CONSOLE ATTACKS");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);

    // Container para controles
    lv_obj_t* controlPanel = lv_obj_create(consoleScreen);
    lv_obj_set_size(controlPanel, lv_pct(100), 100);
    lv_obj_align(controlPanel, LV_ALIGN_TOP_LEFT, 0, 40);
    lv_obj_set_style_bg_color(controlPanel, lv_color_hex(0x333333), 0);

    // Botões de controle - Linha 1
    lv_obj_t* btnScan = lv_btn_create(controlPanel);
    lv_obj_set_size(btnScan, 65, 35);
    lv_obj_align(btnScan, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_t* lblScan = lv_label_create(btnScan);
    lv_label_set_text(lblScan, "Scan");
    lv_obj_add_event_cb(btnScan, scanConsolesEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnPS3 = lv_btn_create(controlPanel);
    lv_obj_set_size(btnPS3, 65, 35);
    lv_obj_align(btnPS3, LV_ALIGN_TOP_LEFT, 75, 5);
    lv_obj_t* lblPS3 = lv_label_create(btnPS3);
    lv_label_set_text(lblPS3, "PS3");
    lv_obj_set_style_bg_color(btnPS3, lv_color_hex(0x0066CC), 0);
    lv_obj_add_event_cb(btnPS3, ps3AttackEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnXbox = lv_btn_create(controlPanel);
    lv_obj_set_size(btnXbox, 65, 35);
    lv_obj_align(btnXbox, LV_ALIGN_TOP_LEFT, 145, 5);
    lv_obj_t* lblXbox = lv_label_create(btnXbox);
    lv_label_set_text(lblXbox, "X360");
    lv_obj_set_style_bg_color(btnXbox, lv_color_hex(0x107C10), 0);
    lv_obj_add_event_cb(btnXbox, xbox360AttackEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnWiiU = lv_btn_create(controlPanel);
    lv_obj_set_size(btnWiiU, 65, 35);
    lv_obj_align(btnWiiU, LV_ALIGN_TOP_LEFT, 215, 5);
    lv_obj_t* lblWiiU = lv_label_create(btnWiiU);
    lv_label_set_text(lblWiiU, "WiiU");
    lv_obj_set_style_bg_color(btnWiiU, lv_color_hex(0xE60012), 0);
    lv_obj_add_event_cb(btnWiiU, wiiuAttackEvent, LV_EVENT_CLICKED, NULL);

    // Linha 2
    lv_obj_t* btnSwitch = lv_btn_create(controlPanel);
    lv_obj_set_size(btnSwitch, 65, 35);
    lv_obj_align(btnSwitch, LV_ALIGN_TOP_LEFT, 5, 50);
    lv_obj_t* lblSwitch = lv_label_create(btnSwitch);
    lv_label_set_text(lblSwitch, "Switch");
    lv_obj_set_style_bg_color(btnSwitch, lv_color_hex(0xE60012), 0);
    lv_obj_add_event_cb(btnSwitch, switchAttackEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnMic = lv_btn_create(controlPanel);
    lv_obj_set_size(btnMic, 65, 35);
    lv_obj_align(btnMic, LV_ALIGN_TOP_LEFT, 75, 50);
    lv_obj_t* lblMic = lv_label_create(btnMic);
    lv_label_set_text(lblMic, "Mic");
    lv_obj_set_style_bg_color(btnMic, lv_color_hex(0xFF8C00), 0);
    lv_obj_add_event_cb(btnMic, micBleedAttackEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnStop = lv_btn_create(controlPanel);
    lv_obj_set_size(btnStop, 65, 35);
    lv_obj_align(btnStop, LV_ALIGN_TOP_LEFT, 145, 50);
    lv_obj_t* lblStop = lv_label_create(btnStop);
    lv_label_set_text(lblStop, "Stop");
    lv_obj_set_style_bg_color(btnStop, lv_color_hex(0xDC143C), 0);
    lv_obj_add_event_cb(btnStop, stopAttackEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnBack = lv_btn_create(controlPanel);
    lv_obj_set_size(btnBack, 65, 35);
    lv_obj_align(btnBack, LV_ALIGN_TOP_LEFT, 215, 50);
    lv_obj_t* lblBack = lv_label_create(btnBack);
    lv_label_set_text(lblBack, "Back");
    lv_obj_add_event_cb(btnBack, backEvent, LV_EVENT_CLICKED, NULL);

    // Status label
    statusLabel = lv_label_create(consoleScreen);
    lv_label_set_text(statusLabel, "Ready");
    lv_obj_align(statusLabel, LV_ALIGN_TOP_LEFT, 10, 145);

    // Barra de progresso
    progressBar = lv_bar_create(consoleScreen);
    lv_obj_set_size(progressBar, lv_pct(80), 10);
    lv_obj_align(progressBar, LV_ALIGN_TOP_LEFT, 10, 165);
    lv_bar_set_range(progressBar, 0, 100);
    lv_bar_set_value(progressBar, 0, LV_ANIM_OFF);
    lv_obj_add_flag(progressBar, LV_OBJ_FLAG_HIDDEN);

    // Container para lista de consoles
    lv_obj_t* listContainer = lv_obj_create(consoleScreen);
    lv_obj_set_size(listContainer, lv_pct(100), lv_pct(45));
    lv_obj_align(listContainer, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    // Lista de consoles
    consoleList = lv_obj_create(listContainer);
    lv_obj_set_size(consoleList, lv_pct(100), lv_pct(100));
    lv_obj_align(consoleList, LV_ALIGN_TOP_LEFT, 0, 0);

    // Scroll da lista
    lv_obj_add_flag(consoleList, LV_OBJ_FLAG_SCROLLABLE);

    // Label vazio inicialmente
    lv_obj_t* emptyLabel = lv_label_create(consoleList);
    lv_label_set_text(emptyLabel, "No consoles found.\nTap 'Scan' to search for\nPS3, Xbox 360, Wii U, Switch.");
    lv_obj_center(emptyLabel);
}

void showConsoleMenu() {
    if (consoleMenuActive) return;

    createConsoleMenu();
    consoleAttackManager.begin();
}

// ============================================================================
// INTEGRAÇÃO COM MENU PRINCIPAL
// ============================================================================

extern "C" void consoleMenuFunction() {
    showConsoleMenu();
}

// Função para ser chamada pelo loop principal
void updateConsoleInterface() {
    if (consoleMenuActive) {
        updateConsoleMenu();
    }
}

// Class Implementation
#include "ConsoleMenu.h"

ConsoleMenu consoleMenu;

ConsoleMenu::ConsoleMenu() : MenuItemInterface("Consoles") {}

void ConsoleMenu::optionsMenu() {
    consoleMenuFunction();
}

void ConsoleMenu::drawIcon(float scale) {
    clearIconArea();
    tft.fillRoundRect(iconCenterX - 20 * scale, iconCenterY - 15 * scale, 40 * scale, 30 * scale, 4 * scale, leleConfig.priColor);
    tft.fillRect(iconCenterX - 15 * scale, iconCenterY - 10 * scale, 30 * scale, 20 * scale, leleConfig.bgColor);
}

void ConsoleMenu::drawIconImg() {
    // Theme not supported yet
}

bool ConsoleMenu::getTheme() { 
    return false;
}
