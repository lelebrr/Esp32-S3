#pragma once

/**
 * @file ui_settings_widgets.h
 * @brief Widgets reutilizáveis para as telas de configurações
 */

#include <lvgl.h>

// ============================================================
// TIPOS DE CALLBACK
// ============================================================
typedef void (*setting_slider_cb_t)(int32_t value);
typedef void (*setting_switch_cb_t)(bool checked);
typedef void (*setting_dropdown_cb_t)(uint16_t selected);
typedef void (*setting_button_cb_t)(void);
typedef void (*setting_text_cb_t)(const char *text);

// ============================================================
// CORES NEON PARA COLOR PICKER (32 cores)
// ============================================================
static const uint32_t neon_colors[32] = {
    0x00FF00, 0x00FF7F, 0x00FFFF, 0x007FFF, // Verdes/Cianos
    0x0000FF, 0x7F00FF, 0xFF00FF, 0xFF007F, // Azuis/Magentas
    0xFF0000, 0xFF7F00, 0xFFFF00, 0x7FFF00, // Vermelhos/Amarelos
    0x00FF41, 0x00D4FF, 0xFF00AA, 0xAA00FF, // Neons especiais
    0x39FF14, 0x00FFEF, 0xFE019A, 0xBE03FD, // Neons brilhantes
    0xFF073A, 0xFE4164, 0x04D9FF, 0x9DFF00, // Neons vibrantes
    0xCFFF04, 0xFCFE00, 0xFFCB00, 0xFF6700, // Amarelos/Laranjas
    0xFFFFFF, 0xC0C0C0, 0x808080, 0x404040  // Neutros
};

// ============================================================
// FUNÇÕES DE CRIAÇÃO DE WIDGETS
// ============================================================

/**
 * @brief Cria uma linha de configuração com slider
 * @param parent Container pai
 * @param label Texto do label
 * @param icon Ícone LVGL (LV_SYMBOL_*)
 * @param min Valor mínimo
 * @param max Valor máximo
 * @param current Valor atual
 * @param cb Callback quando valor muda
 * @return Ponteiro para o slider criado
 */
lv_obj_t *ui_create_slider_row(lv_obj_t *parent, const char *label,
                               const char *icon, int32_t min, int32_t max,
                               int32_t current, setting_slider_cb_t cb);

/**
 * @brief Cria uma linha de configuração com switch on/off
 * @param parent Container pai
 * @param label Texto do label
 * @param icon Ícone LVGL
 * @param checked Estado inicial
 * @param cb Callback quando estado muda
 * @return Ponteiro para o switch criado
 */
lv_obj_t *ui_create_switch_row(lv_obj_t *parent, const char *label,
                               const char *icon, bool checked,
                               setting_switch_cb_t cb);

/**
 * @brief Cria uma linha de configuração com dropdown
 * @param parent Container pai
 * @param label Texto do label
 * @param icon Ícone LVGL
 * @param options Lista de opções separadas por \n
 * @param selected Índice selecionado
 * @param cb Callback quando seleção muda
 * @return Ponteiro para o dropdown criado
 */
lv_obj_t *ui_create_dropdown_row(lv_obj_t *parent, const char *label,
                                 const char *icon, const char *options,
                                 uint16_t selected, setting_dropdown_cb_t cb);

/**
 * @brief Cria uma linha de configuração com botão de ação
 * @param parent Container pai
 * @param label Texto do label
 * @param icon Ícone LVGL
 * @param btn_text Texto do botão
 * @param cb Callback quando botão é clicado
 * @return Ponteiro para o botão criado
 */
lv_obj_t *ui_create_button_row(lv_obj_t *parent, const char *label,
                               const char *icon, const char *btn_text,
                               setting_button_cb_t cb);

/**
 * @brief Cria um seletor de cor com 32 cores neon
 * @param parent Container pai
 * @param label Texto do label
 * @param current_idx Índice da cor atual (0-31)
 * @param cb Callback quando cor muda
 * @return Ponteiro para o container do color picker
 */
lv_obj_t *ui_create_color_picker(lv_obj_t *parent, const char *label,
                                 uint8_t current_idx, setting_dropdown_cb_t cb);

/**
 * @brief Cria uma linha somente para exibição (read-only)
 * @param parent Container pai
 * @param label Texto do label
 * @param icon Ícone LVGL
 * @param value Valor a exibir
 * @return Ponteiro para o label de valor
 */
lv_obj_t *ui_create_info_row(lv_obj_t *parent, const char *label,
                             const char *icon, const char *value);

/**
 * @brief Cria um header de seção
 * @param parent Container pai
 * @param title Título da seção
 * @return Ponteiro para o label criado
 */
lv_obj_t *ui_create_section_header(lv_obj_t *parent, const char *title);

/**
 * @brief Cria um container scrollável para settings
 * @param parent Tela pai
 * @param title Título da tela
 * @param back_cb Callback do botão voltar
 * @return Ponteiro para o container scrollável
 */
lv_obj_t *ui_create_settings_screen(lv_obj_t **screen_out, const char *title,
                                    lv_event_cb_t back_cb);

/**
 * @brief Atualiza o valor exibido em um info row
 * @param label_obj Ponteiro do label retornado por ui_create_info_row
 * @param value Novo valor
 */
void ui_update_info_row(lv_obj_t *label_obj, const char *value);
