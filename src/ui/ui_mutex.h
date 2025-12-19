/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - UI Mutex
 * Thread-safety para LVGL evitar crashes de tela
 * ═══════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// ═══════════════════════════════════════════════════════════════════════════
// MUTEX GLOBAL
// ═══════════════════════════════════════════════════════════════════════════

extern SemaphoreHandle_t g_ui_mutex;

/**
 * @brief Inicializa o mutex da UI
 * @return true se inicializado com sucesso
 */
bool ui_mutex_init();

/**
 * @brief Adquire lock da UI
 * @param timeout Tempo máximo de espera
 * @return true se lock adquirido
 */
bool ui_lock(TickType_t timeout = portMAX_DELAY);

/**
 * @brief Libera lock da UI
 */
void ui_unlock();

/**
 * @brief Verifica se o mutex está travado
 * @return true se travado
 */
bool ui_is_locked();

// ═══════════════════════════════════════════════════════════════════════════
// RAII HELPER - Auto-lock/unlock via escopo
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief Classe RAII para auto-lock/unlock da UI
 * 
 * Uso:
 *   {
 *       UILock lock;  // Adquire lock automaticamente
 *       lv_label_set_text(...);
 *   }  // Lock liberado automaticamente ao sair do escopo
 */
struct UILock {
    bool acquired;
    
    UILock(TickType_t timeout = portMAX_DELAY) {
        acquired = ui_lock(timeout);
    }
    
    ~UILock() {
        if (acquired) {
            ui_unlock();
        }
    }
    
    // Previne cópias
    UILock(const UILock&) = delete;
    UILock& operator=(const UILock&) = delete;
    
    // Permite verificar se lock foi adquirido
    operator bool() const { return acquired; }
};

// ═══════════════════════════════════════════════════════════════════════════
// MACROS DE CONVENIÊNCIA
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief Executa código LVGL com lock automático
 * 
 * Uso:
 *   UI_SAFE {
 *       lv_label_set_text(label, "Hello");
 *   }
 */
#define UI_SAFE if (UILock _ui_lock_; _ui_lock_)

/**
 * @brief Tenta lock com timeout, senão pula
 * 
 * Uso:
 *   UI_TRY(100) {
 *       lv_label_set_text(label, "Hello");
 *   }
 */
#define UI_TRY(timeout_ms) if (UILock _ui_lock_(pdMS_TO_TICKS(timeout_ms)); _ui_lock_)
