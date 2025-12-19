#pragma once

#include <lvgl.h>

/**
 * @brief Troca de tela com animação
 * @param target_screen Tela alvo (lv_obj_t*)
 * @param anim_type Tipo de animação (LV_SCR_LOAD_ANIM_...)
 * @param time Tempo da animação em ms
 * @param delay Delay antes de iniciar
 */
void ui_switch_screen(lv_obj_t *target_screen,
                      lv_scr_load_anim_t anim_type = LV_SCR_LOAD_ANIM_FADE_IN,
                      uint32_t time = 200, uint32_t delay = 0);

/**
 * @brief Aplica efeito de vidro (Glassmorphism) a um objeto
 * @param obj Objeto a aplicar o efeito
 */
void ui_apply_glass_effect(lv_obj_t *obj);

/**
 * @brief Cria o layout de 3 zonas (Status Bar, Conteúdo, Nav Bar)
 *
 * @param parent Tela pai
 * @param content_area Ponteiro para armazenar a referência da área de conteúdo
 * (meio)
 */
void ui_create_3_zone_layout(lv_obj_t *parent, lv_obj_t **content_area);

// Styles
extern lv_style_t style_card_neon;
extern lv_style_t style_text_large;
extern lv_style_t style_text_title;
extern lv_style_t style_nav_btn;

void ui_init_global_styles();

/**
 * @brief Cria padrão de botão Back (Tip 7)
 * @param parent Parent object
 * @param cb Callback on click
 */
void ui_create_back_btn(lv_obj_t *parent, lv_event_cb_t cb);
