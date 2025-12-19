/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - UI Mutex Implementation
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "ui_mutex.h"
// #include "../usb_serial_compat.h" // File not found - disabled
#include <debug_log.h>

// Mutex global
SemaphoreHandle_t g_ui_mutex = nullptr;

bool ui_mutex_init() {
    if (g_ui_mutex != nullptr) {
        return true; // Já inicializado
    }

    g_ui_mutex = xSemaphoreCreateMutex();

    if (g_ui_mutex == nullptr) {
        LOG_ERROR("UI: Falha ao criar mutex da UI");
        return false;
    }

    LOG_UI("UI: Mutex inicializado");
    return true;
}

bool ui_lock(TickType_t timeout) {
    if (g_ui_mutex == nullptr) { return false; }

    return xSemaphoreTake(g_ui_mutex, timeout) == pdTRUE;
}

void ui_unlock() {
    if (g_ui_mutex != nullptr) { xSemaphoreGive(g_ui_mutex); }
}

bool ui_is_locked() {
    if (g_ui_mutex == nullptr) { return false; }

    // Tenta adquirir lock sem espera
    if (xSemaphoreTake(g_ui_mutex, 0) == pdTRUE) {
        // Conseguiu, então não estava travado - libera imediatamente
        xSemaphoreGive(g_ui_mutex);
        return false;
    }

    return true; // Não conseguiu, então está travado
}
