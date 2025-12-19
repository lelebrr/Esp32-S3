/**
 * @file settings_submenus_2.cpp
 * @brief Settings submenu implementations (Network, Attacks, System)
 *
 * TODO: These submenus require a proper state management system.
 * Currently using placeholder implementations.
 */

#include "globals.h"
#include "settings_menu_v2.h"
#include <lvgl.h>

// ═══════════════════════════════════════════════════════════════════════════
// SUBMENU 5: REDE & WIRELESS
// ═══════════════════════════════════════════════════════════════════════════
lv_obj_t *SettingsMenuV2::createNetworkSubmenu(lv_obj_t *parent) {
    // TODO: Connect to actual state management
    createSlider(parent, "Potência TX (dBm)", 8, 20, 20, nullptr);
    createDropdown(parent, "Canal WiFi", "Auto\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13", 0, nullptr);
    createSwitch(parent, "MAC Spoof no Boot", false, nullptr);
    createSwitch(parent, "Usar MAC Custom", false, nullptr);
    createButton(parent, "MAC Customizado", "Editar", nullptr);
    createSwitch(parent, "2.4 GHz Ativo", true, nullptr);

    createSwitch(parent, "Beacon Spam", false, nullptr);
    createSlider(parent, "Velocidade Beacon", 1, 10, 5, nullptr);
    createSlider(parent, "Qtd SSIDs", 5, 50, 20, nullptr);
    createSwitch(parent, "Evil Twin Auto", false, nullptr);
    createSwitch(parent, "Karma/Mana Attack", false, nullptr);
    createSwitch(parent, "Modo Monitor Only", false, nullptr);
    createSwitch(parent, "BLE Spam", false, nullptr);
    createDropdown(parent, "Tipo BLE Spam", "Apple\nWindows\nSamsung\nTodos", 0, nullptr);
    createSwitch(parent, "Modo Promíscuo", false, nullptr);
    return parent;
}

// ═══════════════════════════════════════════════════════════════════════════
// SUBMENU 6: ATAQUES & AUTOMAÇÃO
// ═══════════════════════════════════════════════════════════════════════════
lv_obj_t *SettingsMenuV2::createAttacksSubmenu(lv_obj_t *parent) {
    // TODO: Connect to actual state management
    createSwitch(parent, "WPS Pixie Auto", false, nullptr);
    createSwitch(parent, "WPS Brute-force", false, nullptr);
    createSlider(parent, "WPS Flood (1-10)", 1, 10, 5, nullptr);
    createSlider(parent, "Rogue APs (1-50)", 1, 50, 10, nullptr);
    createSwitch(parent, "Auto-Capture Novas", true, nullptr);
    createSwitch(parent, "Auto-Salvar PCAP", true, nullptr);
    createSwitch(parent, "Ataque Favoritos", false, nullptr);
    createSwitch(parent, "Modo Insano (60s)", false, nullptr);

    createSwitch(parent, "Portal Cativo Auto", false, nullptr);
    createDropdown(
        parent, "Template Portal", "Login\nFacebook\nGoogle\nWiFi Grátis\nHotel\nAeroporto", 0, nullptr
    );
    createSwitch(parent, "BadUSB Auto", false, nullptr);
    return parent;
}

// ═══════════════════════════════════════════════════════════════════════════
// SUBMENU 7: SISTEMA & ENERGIA
// ═══════════════════════════════════════════════════════════════════════════
lv_obj_t *SettingsMenuV2::createSystemSubmenu(lv_obj_t *parent) {
    // TODO: Connect to actual state management
    createSlider(parent, "Deep Sleep (min)", 0, 255, 0, nullptr);
    createSwitch(parent, "Acordar com Toque", true, nullptr);
    createSwitch(parent, "Acordar com Shake", false, nullptr);
    createSwitch(parent, "Boot Stealth", false, nullptr);
    createSlider(parent, "Fuso Horário", -12, 14, -3, nullptr);
    createButton(parent, "Backup Config", "Salvar", nullptr);
    createButton(parent, "Restaurar Config", "Carregar", nullptr);
    createButton(parent, "OTA Update", "Atualizar", nullptr);
    createButton(parent, "Factory Reset", "⚠️ RESET", nullptr);
    createButton(parent, "Reiniciar", "Reboot", nullptr);
    return parent;
}
