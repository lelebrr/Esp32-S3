/**
 * @file WPSMenu.cpp
 * @brief Menu de Ataques WPS para Lele Origin
 * @author Lele Origin Team
 */

#include <globals.h>
#include <lvgl.h>
#include <core/display.h>
#include <core/mykeyboard.h>
#include <modules/wps/wps_attacks.h>

// ============================================================================
// VARIÁVEIS GLOBAIS DO MENU
// ============================================================================

static bool wpsMenuActive = false;
static lv_obj_t* wpsScreen = nullptr;
static lv_obj_t* networkList = nullptr;
static lv_obj_t* statusLabel = nullptr;
static lv_obj_t* progressBar = nullptr;

// ============================================================================
// CALLBACKS E EVENTOS
// ============================================================================

static void scanNetworksEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        displayInfo("Scanning for WPS networks...", false);

        if (wpsAttackManager.startScan(30)) {
            lv_label_set_text(statusLabel, "Scanning...");
            lv_obj_clear_flag(progressBar, LV_OBJ_FLAG_HIDDEN);
        } else {
            displayError("Scan already running", true);
        }
    }
}

static void pixieDustEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (wpsAttackManager.getCurrentTarget().ssid.isEmpty()) {
            displayError("Select target first", true);
            return;
        }

        displayInfo("Starting Pixie Dust attack...", false);

        if (wpsAttackManager.startPixieDustAttack()) {
            lv_label_set_text(statusLabel, "Pixie Dust Active");
        } else {
            displayError("Failed to start attack", true);
        }
    }
}

static void reaverBruteEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (wpsAttackManager.getCurrentTarget().ssid.isEmpty()) {
            displayError("Select target first", true);
            return;
        }

        displayInfo("Starting Reaver brute force...", false);

        if (wpsAttackManager.startReaverAttack()) {
            lv_label_set_text(statusLabel, "Reaver Brute Active");
        } else {
            displayError("Failed to start attack", true);
        }
    }
}

static void evilTwinEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (wpsAttackManager.getCurrentTarget().ssid.isEmpty()) {
            displayError("Select target first", true);
            return;
        }

        displayInfo("Starting Evil Twin attack...", false);

        if (wpsAttackManager.startEvilTwin()) {
            lv_label_set_text(statusLabel, "Evil Twin Active");
        } else {
            displayError("Failed to start attack", true);
        }
    }
}

static void saveDataEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (wpsAttackManager.saveCapturedData()) {
            displaySuccess("Data saved to SD card", true);
        } else {
            displayError("Failed to save data", true);
        }
    }
}

static void stopAttackEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        wpsAttackManager.stopAttack();
        lv_label_set_text(statusLabel, "Attack stopped");
        displayInfo("Attack stopped", true);
    }
}

static void networkSelectEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_t* btn = lv_event_get_target(e);
        uint32_t idx = lv_obj_get_index(btn);

        auto& networks = wpsAttackManager.getNetworks();
        if (idx < networks.size()) {
            wpsAttackManager.selectTarget(networks[idx]);
            displaySuccess("Target: " + networks[idx].ssid, true);
            lv_label_set_text(statusLabel, ("Target: " + networks[idx].ssid).c_str());
        }
    }
}

static void backEvent(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        wpsMenuActive = false;
        lv_obj_del(wpsScreen);
        wpsScreen = nullptr;
    }
}

// ============================================================================
// FUNÇÕES DO MENU
// ============================================================================

void updateWPSMenu() {
    if (!wpsMenuActive || !wpsScreen) return;

    // Atualiza lista de redes
    auto& networks = wpsAttackManager.getNetworks();

    // Limpa lista atual
    lv_obj_clean(networkList);

    // Adiciona redes encontradas
    for (size_t i = 0; i < networks.size(); i++) {
        const auto& net = networks[i];

        lv_obj_t* btn = lv_btn_create(networkList);
        lv_obj_set_size(btn, lv_pct(95), 50);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, i * 55);

        // Label com informações da rede
        lv_obj_t* label = lv_label_create(btn);
        String info = net.ssid + " [" + String(net.rssi) + "dBm]";
        if (net.wpsEnabled) {
            info += " WPS";
            if (net.wpsLocked) info += "🔒";
            else info += "✅";
        }
        lv_label_set_text(label, info.c_str());

        // Armazena índice da rede
        lv_obj_set_user_data(btn, (void*)i);
        lv_obj_add_event_cb(btn, networkSelectEvent, LV_EVENT_CLICKED, nullptr);
    }

    // Atualiza barra de progresso se estiver escaneando
    if (wpsAttackManager.getState() == WPS_SCANNING) {
        int progress = 0; // TODO: implementar getScanProgress
        lv_bar_set_value(progressBar, progress, LV_ANIM_ON);
        lv_label_set_text(statusLabel, "Scanning...");
    } else if (wpsAttackManager.getState() == WPS_PIXIE_DUST) {
        lv_label_set_text(statusLabel, "Pixie Dust Active");
    } else if (wpsAttackManager.getState() == WPS_REAVER_BRUTE) {
        lv_label_set_text(statusLabel, "Reaver Brute Active");
    } else if (wpsAttackManager.getState() == WPS_EVIL_TWIN) {
        lv_label_set_text(statusLabel, "Evil Twin Active");
    } else {
        lv_label_set_text(statusLabel, "Ready");
    }
}

void createWPSMenu() {
    wpsMenuActive = true;

    // Cria tela principal
    wpsScreen = lv_obj_create(NULL);
    lv_scr_load(wpsScreen);

    // Título
    lv_obj_t* title = lv_label_create(wpsScreen);
    lv_label_set_text(title, "WPS ATTACK MODE");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);

    // Container para controles
    lv_obj_t* controlPanel = lv_obj_create(wpsScreen);
    lv_obj_set_size(controlPanel, lv_pct(100), 80);
    lv_obj_align(controlPanel, LV_ALIGN_TOP_LEFT, 0, 40);
    lv_obj_set_style_bg_color(controlPanel, lv_color_hex(0x333333), 0);

    // Botões de controle
    lv_obj_t* btnScan = lv_btn_create(controlPanel);
    lv_obj_set_size(btnScan, 70, 35);
    lv_obj_align(btnScan, LV_ALIGN_TOP_LEFT, 10, 5);
    lv_obj_t* lblScan = lv_label_create(btnScan);
    lv_label_set_text(lblScan, "Scan");
    lv_obj_add_event_cb(btnScan, scanNetworksEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnPixie = lv_btn_create(controlPanel);
    lv_obj_set_size(btnPixie, 70, 35);
    lv_obj_align(btnPixie, LV_ALIGN_TOP_LEFT, 90, 5);
    lv_obj_t* lblPixie = lv_label_create(btnPixie);
    lv_label_set_text(lblPixie, "Pixie");
    lv_obj_set_style_bg_color(btnPixie, lv_color_hex(0xFF4500), 0);
    lv_obj_add_event_cb(btnPixie, pixieDustEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnReaver = lv_btn_create(controlPanel);
    lv_obj_set_size(btnReaver, 70, 35);
    lv_obj_align(btnReaver, LV_ALIGN_TOP_LEFT, 170, 5);
    lv_obj_t* lblReaver = lv_label_create(btnReaver);
    lv_label_set_text(lblReaver, "Reaver");
    lv_obj_set_style_bg_color(btnReaver, lv_color_hex(0xFF1493), 0);
    lv_obj_add_event_cb(btnReaver, reaverBruteEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnEvil = lv_btn_create(controlPanel);
    lv_obj_set_size(btnEvil, 70, 35);
    lv_obj_align(btnEvil, LV_ALIGN_TOP_LEFT, 250, 5);
    lv_obj_t* lblEvil = lv_label_create(btnEvil);
    lv_label_set_text(lblEvil, "EvilTwin");
    lv_obj_set_style_bg_color(btnEvil, lv_color_hex(0x8A2BE2), 0);
    lv_obj_add_event_cb(btnEvil, evilTwinEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnStop = lv_btn_create(controlPanel);
    lv_obj_set_size(btnStop, 70, 35);
    lv_obj_align(btnStop, LV_ALIGN_TOP_LEFT, 10, 40);
    lv_obj_t* lblStop = lv_label_create(btnStop);
    lv_label_set_text(lblStop, "Stop");
    lv_obj_set_style_bg_color(btnStop, lv_color_hex(0xDC143C), 0);
    lv_obj_add_event_cb(btnStop, stopAttackEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnSave = lv_btn_create(controlPanel);
    lv_obj_set_size(btnSave, 70, 35);
    lv_obj_align(btnSave, LV_ALIGN_TOP_LEFT, 90, 40);
    lv_obj_t* lblSave = lv_label_create(btnSave);
    lv_label_set_text(lblSave, "Save");
    lv_obj_set_style_bg_color(btnSave, lv_color_hex(0x32CD32), 0);
    lv_obj_add_event_cb(btnSave, saveDataEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnBack = lv_btn_create(controlPanel);
    lv_obj_set_size(btnBack, 70, 35);
    lv_obj_align(btnBack, LV_ALIGN_TOP_LEFT, 250, 40);
    lv_obj_t* lblBack = lv_label_create(btnBack);
    lv_label_set_text(lblBack, "Back");
    lv_obj_add_event_cb(btnBack, backEvent, LV_EVENT_CLICKED, NULL);

    // Status label
    statusLabel = lv_label_create(wpsScreen);
    lv_label_set_text(statusLabel, "Ready");
    lv_obj_align(statusLabel, LV_ALIGN_TOP_LEFT, 10, 125);

    // Barra de progresso
    progressBar = lv_bar_create(wpsScreen);
    lv_obj_set_size(progressBar, lv_pct(80), 10);
    lv_obj_align(progressBar, LV_ALIGN_TOP_LEFT, 10, 145);
    lv_bar_set_range(progressBar, 0, 100);
    lv_bar_set_value(progressBar, 0, LV_ANIM_OFF);
    lv_obj_add_flag(progressBar, LV_OBJ_FLAG_HIDDEN);

    // Container para lista de redes
    lv_obj_t* listContainer = lv_obj_create(wpsScreen);
    lv_obj_set_size(listContainer, lv_pct(100), lv_pct(55));
    lv_obj_align(listContainer, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    // Lista de redes
    networkList = lv_obj_create(listContainer);
    lv_obj_set_size(networkList, lv_pct(100), lv_pct(100));
    lv_obj_align(networkList, LV_ALIGN_TOP_LEFT, 0, 0);

    // Scroll da lista
    lv_obj_add_flag(networkList, LV_OBJ_FLAG_SCROLLABLE);

    // Label vazio inicialmente
    lv_obj_t* emptyLabel = lv_label_create(networkList);
    lv_label_set_text(emptyLabel, "No networks found.\nTap 'Scan' to search.");
    lv_obj_center(emptyLabel);
}

void showWPSMenu() {
    if (wpsMenuActive) return;

    createWPSMenu();
    wpsAttackManager.begin();
}

// ============================================================================
// INTEGRAÇÃO COM MENU PRINCIPAL
// ============================================================================

extern "C" void wpsMenuFunction() {
    showWPSMenu();
}

// Função para ser chamada pelo loop principal
void updateWPSInterface() {
    if (wpsMenuActive) {
        updateWPSMenu();
    }
}
