#pragma once

/**
 * @file ui_handshakes_screen.h
 * @brief Tela de handshakes capturados
 */

// #include "../../plugins/plugin_base.h"
#include <Arduino.h>
#include <lvgl.h>

// ═══════════════════════════════════════════════════════════════════════════
// HANDSHAKE DATA STRUCTURE
// ═══════════════════════════════════════════════════════════════════════════
struct __attribute__((packed)) HandshakeData {
    char ssid[33];      // SSID do AP
    uint8_t bssid[6];   // MAC do AP
    uint8_t station[6]; // MAC da estação
    uint32_t timestamp; // Quando foi capturado
    uint16_t channel;   // Canal
    int8_t rssi;        // Força do sinal
    bool isSaved;       // Se foi salvo em arquivo
    bool pmkid;         // Se é PMKID (vs 4-way handshake)
    char filename[32];  // Nome do arquivo PCAP
};

class HandshakesScreen {
public:
    HandshakesScreen();

    void create(lv_obj_t *parent);
    void show();
    void hide();
    void update();

    void addHandshake(const HandshakeData &hs);
    void clearAll();

private:
    lv_obj_t *_screen;
    lv_obj_t *_list;
    lv_obj_t *_lblCount;
    lv_obj_t *_btnExport;
    lv_obj_t *_btnClear;
    lv_obj_t *_btnBack;

    HandshakeData _handshakes[32]; // Reduced from 100 to save ~6KB DRAM
    int _count;

    static void onExportClick(lv_event_t *e);
    static void onClearClick(lv_event_t *e);
    static void onBackClick(lv_event_t *e);

    void populateList();
};

extern HandshakesScreen handshakesScreen;
