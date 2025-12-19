#ifndef __BOOT_HW_SCAN_H__
#define __BOOT_HW_SCAN_H__

#include <Arduino.h>
#include <globals.h>

// Status de hardware
enum HWStatus { HW_OFFLINE = 0, HW_ONLINE = 1, HW_ERROR = 2, HW_NOT_CONFIGURED = 3 };

// Estrutura para módulos de hardware
struct HWModule {
    const char *name;
    const char *icon; // Emoji/símbolo
    HWStatus status;
    const char *details;
};

// Estrutura para arquivos/pastas obrigatórios no SD
struct SDRequiredItem {
    const char *path;
    bool isDir;
    const char *description;
};

// Funções principais
void boot_screen_hw();      // Tela de boot completa com scan
void boot_hw_scan();        // Escaneia hardware
void boot_sd_check();       // Verifica e cria estrutura SD
void boot_show_results();   // Mostra resultados e aguarda tecla
void boot_draw_animation(); // Animação de logo

// Funções auxiliares
bool detect_cc1101();
bool detect_nrf24();
bool detect_rfid();
bool detect_gps();
bool detect_sd_card();
bool detect_touch();

// Gerenciamento SD Card
bool sd_verify_structure();
bool sd_create_structure();
void sd_rebuild_structure();

// Mostrar status de módulo
void boot_show_module_status(const char *name, HWStatus status, int y);

// Quantidade de módulos
#define HW_MODULE_COUNT 9

// Variáveis globais de resultado
extern HWModule hwModules[HW_MODULE_COUNT];
extern int hwModulesOnline;
extern int hwModulesOffline;
extern bool sdCardReady;

#endif
