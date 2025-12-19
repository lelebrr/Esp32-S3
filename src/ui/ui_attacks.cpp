/**
 * @file ui_attacks.cpp
 * @brief Tela de ataques WiFi do WavePwn
 */

#include "ui_attacks.h"
#include "../hardware/ble_driver.h"
#include "../hardware/wifi_driver.h"
#include "../wifi/captive_portal.h"
#include "../wifi/wifi_attacks.h"
#include "../wifi/wps_attacks.h"
#include "globals.h"
#include "ui_helpers.h"
#include "ui_main.h"
#include "ui_wifi_chaos.h"
#include <debug_log.h>


// Objetos LVGL
static lv_obj_t *scr_attacks = nullptr;
static lv_obj_t *list_networks = nullptr;
static lv_obj_t *lbl_status = nullptr;
static lv_obj_t *btn_back = nullptr;

// Cores
#define UI_ATK_BG lv_color_hex(0x0D1117)
#define UI_ATK_PANEL lv_color_hex(0x161B22)
#define UI_ATK_PRIMARY lv_color_hex(0x238636)
#define UI_ATK_DANGER lv_color_hex(0xDA3633)
#define UI_ATK_WARNING lv_color_hex(0xD29922)
#define UI_ATK_TEXT lv_color_hex(0xC9D1D9)

// Rede selecionada
static int selected_network = -1;
static bool attack_screen_active = false;

/**
 * @brief Callback do botão Voltar
 */
static void btn_back_cb(lv_event_t *e) {
    attack_screen_active = false;
    ui_main_show();
}

/**
 * @brief Callback de seleção de rede
 */
static void network_selected_cb(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    uint32_t idx = lv_obj_get_index(btn);
    selected_network = idx;

    LOG_WIFI("Rede selecionada: %d", idx);

    // Atualiza status
    if (lbl_status) {
        WiFiNetwork net = wifi_driver.getNetwork(idx);
        lv_label_set_text_fmt(lbl_status, "Alvo: %s (CH%d)", net.ssid, net.channel);
    }
}

/**
 * @brief Callback botão Deauth
 */
static void btn_deauth_cb(lv_event_t *e) {
    if (selected_network < 0) {
        if (lbl_status) lv_label_set_text(lbl_status, "Selecione uma rede!");
        return;
    }

    WiFiNetwork net = wifi_driver.getNetwork(selected_network);
    wifi_attacks.setTarget(net.bssid, net.ssid, net.channel);
    wifi_attacks.startDeauth(100);

    if (lbl_status) lv_label_set_text(lbl_status, "Deauth ATIVO!");
    LOG_WIFI("Deauth em %s", net.ssid);
}

/**
 * @brief Callback botão Handshake
 */
static void btn_handshake_cb(lv_event_t *e) {
    if (selected_network < 0) {
        if (lbl_status) lv_label_set_text(lbl_status, "Selecione uma rede!");
        return;
    }

    WiFiNetwork net = wifi_driver.getNetwork(selected_network);
    wifi_attacks.setTarget(net.bssid, net.ssid, net.channel);
    wifi_attacks.startHandshakeCapture();

    if (lbl_status) lv_label_set_text(lbl_status, "Capturando handshakes...");
}

/**
 * @brief Callback botão Beacon
 */
static void btn_beacon_cb(lv_event_t *e) {
    wifi_attacks.startBeaconFlood(nullptr, 0);
    if (lbl_status) lv_label_set_text(lbl_status, "Beacon Flood ATIVO!");
}

/**
 * @brief Callback botão Stop
 */
static void btn_stop_cb(lv_event_t *e) {
    wifi_attacks.stopAttack();
    wps_attacks.stopAttack();
    if (lbl_status) lv_label_set_text(lbl_status, "Ataques parados");
}

/**
 * @brief Callback botão WPS Pixie Dust
 */
static void btn_wps_cb(lv_event_t *e) {
    if (selected_network < 0) {
        if (lbl_status) lv_label_set_text(lbl_status, "Selecione uma rede WPS!");
        return;
    }

    WiFiNetwork net = wifi_driver.getNetwork(selected_network);
    wps_attacks.setTarget(net.bssid, net.ssid, net.channel);
    wps_attacks.startPixieDust();

    if (lbl_status) lv_label_set_text(lbl_status, "WPS Pixie Dust...");
    LOG_WIFI("WPS Pixie em %s", net.ssid);
}

/**
 * @brief Callback botão Evil Twin
 */
static void btn_eviltwin_cb(lv_event_t *e) {
    if (selected_network < 0) {
        if (lbl_status) lv_label_set_text(lbl_status, "Selecione uma rede!");
        return;
    }

    WiFiNetwork net = wifi_driver.getNetwork(selected_network);
    int tmpl_idx = random(0, 21); // Random template from available 21
    wifi_attacks.startEvilTwin(net.ssid, tmpl_idx);

    if (lbl_status) lv_label_set_text_fmt(lbl_status, "Evil Twin: T%d", tmpl_idx);
    LOG_WIFI("Evil Twin: %s (Template %d)", net.ssid, tmpl_idx);
}

/**
 * @brief Callback botão BLE Spam
 */
static void btn_blespam_cb(lv_event_t *e) {
    // Toggle BLE spam
    if (ble_driver.isSpamming()) {
        ble_driver.stopSpam();
        g_state.ble_enabled = false;
        if (lbl_status) lv_label_set_text(lbl_status, "BLE Spam PARADO");
        LOG_WIFI("BLE Spam parado");
    } else {
        // Inicia com ALL para máximo impacto
        g_state.ble_enabled = true;
        ble_driver.startSpam(BLE_SPAM_KITCHEN_SINK);
        if (lbl_status) lv_label_set_text(lbl_status, "BLE Spam ATIVO!");
        LOG_WIFI("BLE Spam iniciado (ALL)");
    }
}

/**
 * @brief Cria a tela de ataques
 */
lv_obj_t *ui_attacks_create() {
    scr_attacks = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(scr_attacks, UI_ATK_BG, 0);

    // Título
    lv_obj_t *title = lv_label_create(scr_attacks);
    lv_label_set_text(title, LV_SYMBOL_WARNING " Ataques WiFi");
    lv_obj_set_style_text_color(title, UI_ATK_DANGER, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Lista de redes
    list_networks = lv_list_create(scr_attacks);
    lv_obj_set_size(list_networks, LCD_WIDTH - 20, 180);
    lv_obj_align(list_networks, LV_ALIGN_TOP_MID, 0, 45);
    ui_apply_glass_effect(list_networks);
    lv_obj_set_style_border_color(list_networks, UI_ATK_PRIMARY, 0);

    // Popula a lista com redes escaneadas
    ui_attacks_refresh();

    // Status label
    lbl_status = lv_label_create(scr_attacks);
    lv_label_set_text(lbl_status, "Selecione uma rede alvo");
    lv_obj_set_style_text_color(lbl_status, UI_ATK_TEXT, 0);
    lv_obj_align(lbl_status, LV_ALIGN_CENTER, 0, 115);

    // Botões de ataque
    lv_obj_t *btn_container = lv_obj_create(scr_attacks);
    lv_obj_set_size(btn_container, LCD_WIDTH - 20, 110);
    lv_obj_align(btn_container, LV_ALIGN_BOTTOM_MID, 0, -50);
    lv_obj_set_style_bg_opa(btn_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btn_container, 0, 0);
    lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(
        btn_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER
    );

    // Deauth
    lv_obj_t *btn_deauth = lv_btn_create(btn_container);
    lv_obj_set_size(btn_deauth, 100, 45);
    lv_obj_set_style_bg_color(btn_deauth, UI_ATK_DANGER, 0);
    lv_obj_add_event_cb(btn_deauth, btn_deauth_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *lbl = lv_label_create(btn_deauth);
    lv_label_set_text(lbl, "Deauth");
    lv_obj_center(lbl);

    // Handshake
    lv_obj_t *btn_handshake = lv_btn_create(btn_container);
    lv_obj_set_size(btn_handshake, 100, 45);
    lv_obj_set_style_bg_color(btn_handshake, UI_ATK_WARNING, 0);
    lv_obj_add_event_cb(btn_handshake, btn_handshake_cb, LV_EVENT_CLICKED, nullptr);
    lbl = lv_label_create(btn_handshake);
    lv_label_set_text(lbl, "Capture");
    lv_obj_center(lbl);

    // Beacon
    lv_obj_t *btn_beacon = lv_btn_create(btn_container);
    lv_obj_set_size(btn_beacon, 100, 45);
    lv_obj_set_style_bg_color(btn_beacon, UI_ATK_PRIMARY, 0);
    lv_obj_add_event_cb(btn_beacon, btn_beacon_cb, LV_EVENT_CLICKED, nullptr);
    lbl = lv_label_create(btn_beacon);
    lv_label_set_text(lbl, "Beacon");
    lv_obj_center(lbl);

    // Stop
    lv_obj_t *btn_stop = lv_btn_create(btn_container);
    lv_obj_set_size(btn_stop, 100, 45);
    lv_obj_set_style_bg_color(btn_stop, UI_ATK_PANEL, 0);
    lv_obj_add_event_cb(btn_stop, btn_stop_cb, LV_EVENT_CLICKED, nullptr);
    lbl = lv_label_create(btn_stop);
    lv_label_set_text(lbl, "STOP");
    lv_obj_center(lbl);

    // WPS Pixie Dust
    lv_obj_t *btn_wps = lv_btn_create(btn_container);
    lv_obj_set_size(btn_wps, 100, 45);
    lv_obj_set_style_bg_color(btn_wps, lv_color_hex(0x6F42C1), 0); // Purple
    lv_obj_add_event_cb(btn_wps, btn_wps_cb, LV_EVENT_CLICKED, nullptr);
    lbl = lv_label_create(btn_wps);
    lv_label_set_text(lbl, "WPS");
    lv_obj_center(lbl);

    // Evil Twin
    lv_obj_t *btn_evil = lv_btn_create(btn_container);
    lv_obj_set_size(btn_evil, 100, 45);
    lv_obj_set_style_bg_color(btn_evil, lv_color_hex(0xBD2C2C), 0); // Dark Red
    lv_obj_add_event_cb(btn_evil, btn_eviltwin_cb, LV_EVENT_CLICKED, nullptr);
    lbl = lv_label_create(btn_evil);
    lv_label_set_text(lbl, "Evil Twin");
    lv_obj_center(lbl);

    // BLE Spam
    lv_obj_t *btn_ble = lv_btn_create(btn_container);
    lv_obj_set_size(btn_ble, 100, 45);
    lv_obj_set_style_bg_color(btn_ble, lv_color_hex(0x0066FF), 0); // Blue
    lv_obj_add_event_cb(btn_ble, btn_blespam_cb, LV_EVENT_CLICKED, nullptr);
    lbl = lv_label_create(btn_ble);
    lv_label_set_text(lbl, LV_SYMBOL_BLUETOOTH " BLE");
    lv_obj_center(lbl);

    // Chaos Mode
    lv_obj_t *btn_chaos = lv_btn_create(btn_container);
    lv_obj_set_size(btn_chaos, 100, 45);
    lv_obj_set_style_bg_color(btn_chaos, lv_color_hex(0xFF0000), 0); // Red
    lv_obj_add_event_cb(btn_chaos, [](lv_event_t *e) { ui_wifi_chaos_show(); }, LV_EVENT_CLICKED, nullptr);
    lbl = lv_label_create(btn_chaos);
    lv_label_set_text(lbl, "\xEF\x97\x9A CHAOS"); // Fire icon if available or just text
    lv_obj_center(lbl);

    // Botão Voltar
    // Botão Voltar (Standardized Tip 7)
    ui_create_back_btn(scr_attacks, btn_back_cb);

    // Botão Nuke (Tip 8) - Persistent Floating Action Button
    lv_obj_t *btn_nuke = lv_btn_create(scr_attacks);
    lv_obj_set_size(btn_nuke, 72, 72);
    lv_obj_align(btn_nuke, LV_ALIGN_BOTTOM_RIGHT, -15, -50); // Above Nav Bar
    lv_obj_set_style_radius(btn_nuke, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(btn_nuke, UI_ATK_DANGER, 0);
    lv_obj_set_style_bg_opa(btn_nuke, LV_OPA_COVER, 0);
    lv_obj_set_style_shadow_color(btn_nuke, UI_ATK_DANGER, 0);
    lv_obj_set_style_shadow_width(btn_nuke, 20, 0);
    lv_obj_set_style_shadow_spread(btn_nuke, 5, 0);

    lv_obj_t *lbl_nuke = lv_label_create(btn_nuke);
    lv_label_set_text(lbl_nuke, "NUKE");
    lv_obj_set_style_text_font(lbl_nuke, &lv_font_montserrat_14, 0);
    lv_obj_center(lbl_nuke);

    // Pulse animation (Manual or via anim)
    // For now static, can add anim later if requested

    // Callback placeholder
    lv_obj_add_event_cb(
        btn_nuke,
        [](lv_event_t *e) {
            // Nuke action
            LOG_WIFI("NUKE BUTTON PRESSED!");
            lv_obj_t *btn = lv_event_get_target(e);

            // Visual feedback
            static bool nuke_active = false;
            nuke_active = !nuke_active;

            if (nuke_active) {
                wifi_attacks.startOneTapNuke();
                lv_obj_set_style_bg_color(btn, lv_color_hex(0xFF0000), 0);
                lv_obj_set_style_shadow_width(btn, 30, 0); // Intense glow
            } else {
                wifi_attacks.stopAttack();
                lv_obj_set_style_bg_color(btn, UI_ATK_DANGER,
                                          0); // Back to normal red
                lv_obj_set_style_shadow_width(btn, 20, 0);
            }
        },
        LV_EVENT_CLICKED,
        nullptr
    );

    return scr_attacks;
}

void ui_attacks_show() {
    if (!scr_attacks) { ui_attacks_create(); }
    attack_screen_active = true;
    selected_network = -1;
    ui_attacks_refresh(); // Refresh list on show
    ui_switch_screen(scr_attacks);
}

void ui_attacks_refresh() {
    if (!list_networks) return;

    lv_obj_clean(list_networks);

    int num_networks = wifi_driver.getNetworkCount();
    for (int i = 0; i < num_networks && i < 20; i++) {
        WiFiNetwork net = wifi_driver.getNetwork(i);
        char buf[48];
        snprintf(buf, sizeof(buf), "%s (%lddBm)", net.ssid, (long)net.rssi);

        lv_obj_t *btn = lv_list_add_btn(list_networks, LV_SYMBOL_WIFI, buf);
        lv_obj_add_event_cb(btn, network_selected_cb, LV_EVENT_CLICKED, nullptr);

        // Store index in user_data or rely on list index
        // Using list index is tricky if filtered, but here we just append.
        // lv_obj_get_index(btn) works for simple lists
    }

    if (num_networks == 0) {
        lv_list_add_text(list_networks, "Nenhuma rede - faça scan");
    } else if (num_networks < 0) {
        lv_list_add_text(list_networks, "Scan em andamento...");
    }
}

bool ui_attacks_is_active() { return attack_screen_active; }
