/**
 * @file settings_submenus_1.cpp
 * @brief Settings submenu implementations (Display, Dragon, Sound, Notifications)
 *
 * TODO: These submenus require a proper state management system.
 * Currently using placeholder implementations.
 */

#include "globals.h"
#include "settings_menu_v2.h"
#include <lvgl.h>

// Slider value update callback
static void slider_value_cb(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    lv_obj_t *valLbl = (lv_obj_t *)lv_event_get_user_data(e);
    if (valLbl) lv_label_set_text_fmt(valLbl, "%d", (int)lv_slider_get_value(slider));
}

// ═══════════════════════════════════════════════════════════════════════════
// SUBMENU 1: DISPLAY & INTERFACE
// ═══════════════════════════════════════════════════════════════════════════
lv_obj_t *SettingsMenuV2::createDisplaySubmenu(lv_obj_t *parent) {
    // TODO: Connect to actual state management
    createSlider(parent, "Brilho da Tela", 10, 100, 80, slider_value_cb);
    createSlider(parent, "Tempo Até Apagar (s)", 5, 255, 60, slider_value_cb);
    createSwitch(parent, "Always On Display", false, nullptr);
    createButton(parent, "Wallpaper Atual", "Galeria", nullptr);
    createDropdown(
        parent, "Cor da Fonte", "Cyan\nMagenta\nVerde\nAmarelo\nRoxo\nVermelho\nBranco", 0, nullptr
    );
    createSlider(parent, "Transparência Menu", 0, 90, 50, slider_value_cb);
    createDropdown(parent, "Estilo Ícones", "Flutuante\nBorda Neon\nMinimalista", 0, nullptr);
    createDropdown(parent, "Tamanho Ícones", "Pequeno\nMédio\nGrande", 1, nullptr);
    createDropdown(parent, "Tema Geral", "Cyberpunk\nStealth\nRetro\nKawaii", 0, nullptr);
    createSwitch(parent, "Inverter Cores", false, nullptr);
    createSwitch(parent, "Bateria Gigante", false, nullptr);
    createDropdown(parent, "Formato Relógio", "24h\n12h", 0, nullptr);
    createDropdown(
        parent, "Fonte Sistema", "Neuropol\nOrbitron\nShare Tech\nExo 2\nPixel\nOCR-A\nMonospace", 0, nullptr
    );
    createSwitch(parent, "Mostrar FPS", false, nullptr);
    return parent;
}

// ═══════════════════════════════════════════════════════════════════════════
// SUBMENU 2: DRAGÃO & PERSONALIDADE
// ═══════════════════════════════════════════════════════════════════════════
lv_obj_t *SettingsMenuV2::createDragonSubmenu(lv_obj_t *parent) {
    // TODO: Connect to actual state management
    createButton(parent, "Nome do Dragão", "Dragon", nullptr);
    createDropdown(parent, "Personalidade", "Aggressive\nCute\nPsycho\nLazy\nHacker\nEvil", 0, nullptr);
    createDropdown(
        parent, "Cor dos Olhos", "Cyan\nMagenta\nVerde\nAmarelo\nVermelho\nRoxo\nBranco", 0, nullptr
    );
    createDropdown(parent, "Tamanho", "Pequeno\nMédio\nGrande", 1, nullptr);
    createSwitch(parent, "Fala Ativada", true, nullptr);
    createDropdown(parent, "Idioma", "PT-BR\nEnglish", 0, nullptr);
    createDropdown(parent, "Frequência Falas", "Nunca\nRaro\nNormal\nSpam", 2, nullptr);
    createSwitch(parent, "Reações Exageradas", false, nullptr);
    createSwitch(parent, "Modo Pet Only", false, nullptr);
    return parent;
}

// ═══════════════════════════════════════════════════════════════════════════
// SUBMENU 3: SOM & VIBRAÇÃO
// ═══════════════════════════════════════════════════════════════════════════
lv_obj_t *SettingsMenuV2::createSoundSubmenu(lv_obj_t *parent) {
    // TODO: Connect to actual state management
    createSlider(parent, "Volume Master", 0, 100, 70, slider_value_cb);
    createSwitch(parent, "Som ao Handshake", true, nullptr);
    createSwitch(parent, "Som ao Ligar", true, nullptr);
    createSwitch(parent, "Som ao Ataque", true, nullptr);
    createButton(parent, "Biblioteca de Sons", "Abrir", nullptr);
    createButton(parent, "Testar Som", "▶ Play", nullptr);
    createSwitch(parent, "Vibração Master", true, nullptr);
    createDropdown(
        parent, "Padrão Vibração", "Curta ·\nDupla ··\nLonga —\nCoração\nAlerta\nMorse SOS", 0, nullptr
    );
    createButton(parent, "Testar Vibração", "Testar", nullptr);
    createSlider(parent, "Volume Buzzer", 0, 100, 50, slider_value_cb);
    createSwitch(parent, "Som Handshake", true, nullptr);
    createButton(parent, "Upload Sons (Web)", "→", nullptr);
    return parent;
}

// ═══════════════════════════════════════════════════════════════════════════
// SUBMENU 4: NOTIFICAÇÕES VISUAIS
// ═══════════════════════════════════════════════════════════════════════════
lv_obj_t *SettingsMenuV2::createNotificationsSubmenu(lv_obj_t *parent) {
    // TODO: Connect to actual notification config
    createSwitch(parent, "Notificações Visuais", true, nullptr);
    createDropdown(
        parent,
        "Tipo de Efeito",
        "Flash Total\nBordas Neon\nPulso Central\nScanline\nGlitch\nDragão Pisca",
        0,
        nullptr
    );
    createButton(parent, "Cor da Notificação", "Paleta", nullptr);
    createSlider(parent, "Duração (ms)", 300, 3000, 1000, slider_value_cb);
    createDropdown(parent, "Intensidade Fade", "Suave\nMédio\nAgressivo", 1, nullptr);
    createButton(parent, "🔔 TESTAR", "TESTAR", nullptr);
    // Event checkboxes
    createSwitch(parent, "→ Novo Handshake", true, nullptr);
    createSwitch(parent, "→ WPS PIN", true, nullptr);
    createSwitch(parent, "→ BLE Próximo", false, nullptr);
    createSwitch(parent, "→ Bateria < 20%", true, nullptr);
    createSwitch(parent, "→ SD Quase Cheio", true, nullptr);
    createSwitch(parent, "→ Ataque Concluído", true, nullptr);
    return parent;
}
