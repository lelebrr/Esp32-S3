/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Animation Engine (Motor de Animações)
 * Sistema de animações fluidas para LVGL
 * ═══════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <Arduino.h>
#include <debug_log.h>
#include <lvgl.h>


// Tipos de easing
enum EasingType : uint8_t {
    EASE_LINEAR = 0,
    EASE_IN_QUAD,
    EASE_OUT_QUAD,
    EASE_IN_OUT_QUAD,
    EASE_IN_CUBIC,
    EASE_OUT_CUBIC,
    EASE_IN_OUT_CUBIC,
    EASE_IN_BACK,
    EASE_OUT_BACK,
    EASE_IN_OUT_BACK,
    EASE_BOUNCE
};

class AnimationEngine {
public:
    AnimationEngine();

    void begin();

    // Animações predefinidas
    void fadeIn(lv_obj_t *obj, uint32_t duration, uint32_t delay = 0);
    void fadeOut(lv_obj_t *obj, uint32_t duration, uint32_t delay = 0);
    void slideIn(lv_obj_t *obj, lv_dir_t from, uint32_t duration, uint32_t delay = 0);
    void slideOut(lv_obj_t *obj, lv_dir_t to, uint32_t duration, uint32_t delay = 0);
    void scaleIn(lv_obj_t *obj, uint32_t duration, uint32_t delay = 0);
    void scaleOut(lv_obj_t *obj, uint32_t duration, uint32_t delay = 0);
    void bounce(lv_obj_t *obj, uint32_t duration, uint32_t delay = 0);
    void shake(lv_obj_t *obj, uint32_t duration, uint32_t delay = 0);
    void pulse(lv_obj_t *obj, uint32_t duration, uint32_t delay = 0);
    void rotate(lv_obj_t *obj, int16_t angle, uint32_t duration, uint32_t delay = 0);

    // Animação customizada
    void animate(
        lv_obj_t *obj, lv_anim_exec_xcb_t exec_cb, int32_t start, int32_t end, uint32_t duration,
        EasingType easing = EASE_OUT_QUAD, uint32_t delay = 0
    );

    // Sequência de animações
    void startSequence();
    void addToSequence(
        lv_obj_t *obj, lv_anim_exec_xcb_t exec_cb, int32_t start, int32_t end, uint32_t duration,
        EasingType easing = EASE_OUT_QUAD
    );
    void playSequence(bool parallel = false);

    // Controle
    void pauseAll();
    void resumeAll();
    void stopAll();

    // Easing functions
    static lv_anim_path_cb_t getEasingPath(EasingType type);

private:
    lv_anim_t _sequenceAnims[8];
    uint8_t _sequenceCount;
    uint32_t _sequenceDelay;
};

extern AnimationEngine anim_engine;

// ═══════════════════════════════════════════════════════════════════════════
// IMPLEMENTAÇÃO
// ═══════════════════════════════════════════════════════════════════════════

AnimationEngine anim_engine;

// Callbacks de animação
static void anim_set_x_cb(void *obj, int32_t v) { lv_obj_set_x((lv_obj_t *)obj, v); }
static void anim_set_y_cb(void *obj, int32_t v) { lv_obj_set_y((lv_obj_t *)obj, v); }
static void anim_set_opa_cb(void *obj, int32_t v) { lv_obj_set_style_opa((lv_obj_t *)obj, v, 0); }
static void anim_set_scale_cb(void *obj, int32_t v) {
    lv_obj_set_style_transform_width((lv_obj_t *)obj, v - 256, 0);
    lv_obj_set_style_transform_height((lv_obj_t *)obj, v - 256, 0);
}

AnimationEngine::AnimationEngine() {
    _sequenceCount = 0;
    _sequenceDelay = 0;
}

void AnimationEngine::begin() { LOG_INFO("Anim Engine inicializado"); }

void AnimationEngine::fadeIn(lv_obj_t *obj, uint32_t duration, uint32_t delay) {
    animate(obj, anim_set_opa_cb, 0, LV_OPA_COVER, duration, EASE_OUT_QUAD, delay);
}

void AnimationEngine::fadeOut(lv_obj_t *obj, uint32_t duration, uint32_t delay) {
    animate(obj, anim_set_opa_cb, LV_OPA_COVER, 0, duration, EASE_OUT_QUAD, delay);
}

void AnimationEngine::slideIn(lv_obj_t *obj, lv_dir_t from, uint32_t duration, uint32_t delay) {
    int16_t startX = lv_obj_get_x(obj);
    int16_t startY = lv_obj_get_y(obj);

    if (from == LV_DIR_LEFT) {
        lv_obj_set_x(obj, -lv_obj_get_width(obj));
        animate(obj, anim_set_x_cb, -lv_obj_get_width(obj), startX, duration, EASE_OUT_CUBIC, delay);
    } else if (from == LV_DIR_RIGHT) {
        lv_obj_set_x(obj, LV_HOR_RES);
        animate(obj, anim_set_x_cb, LV_HOR_RES, startX, duration, EASE_OUT_CUBIC, delay);
    } else if (from == LV_DIR_TOP) {
        lv_obj_set_y(obj, -lv_obj_get_height(obj));
        animate(obj, anim_set_y_cb, -lv_obj_get_height(obj), startY, duration, EASE_OUT_CUBIC, delay);
    } else if (from == LV_DIR_BOTTOM) {
        lv_obj_set_y(obj, LV_VER_RES);
        animate(obj, anim_set_y_cb, LV_VER_RES, startY, duration, EASE_OUT_CUBIC, delay);
    }
}

void AnimationEngine::slideOut(lv_obj_t *obj, lv_dir_t to, uint32_t duration, uint32_t delay) {
    int16_t startX = lv_obj_get_x(obj);
    int16_t startY = lv_obj_get_y(obj);

    if (to == LV_DIR_LEFT) {
        animate(obj, anim_set_x_cb, startX, -lv_obj_get_width(obj), duration, EASE_IN_CUBIC, delay);
    } else if (to == LV_DIR_RIGHT) {
        animate(obj, anim_set_x_cb, startX, LV_HOR_RES, duration, EASE_IN_CUBIC, delay);
    } else if (to == LV_DIR_TOP) {
        animate(obj, anim_set_y_cb, startY, -lv_obj_get_height(obj), duration, EASE_IN_CUBIC, delay);
    } else if (to == LV_DIR_BOTTOM) {
        animate(obj, anim_set_y_cb, startY, LV_VER_RES, duration, EASE_IN_CUBIC, delay);
    }
}

void AnimationEngine::scaleIn(lv_obj_t *obj, uint32_t duration, uint32_t delay) {
    animate(obj, anim_set_scale_cb, 0, 256, duration, EASE_OUT_BACK, delay);
}

void AnimationEngine::scaleOut(lv_obj_t *obj, uint32_t duration, uint32_t delay) {
    animate(obj, anim_set_scale_cb, 256, 0, duration, EASE_IN_BACK, delay);
}

void AnimationEngine::bounce(lv_obj_t *obj, uint32_t duration, uint32_t delay) {
    int16_t y = lv_obj_get_y(obj);
    animate(obj, anim_set_y_cb, y, y - 20, duration / 2, EASE_OUT_QUAD, delay);
    animate(obj, anim_set_y_cb, y - 20, y, duration / 2, EASE_BOUNCE, delay + duration / 2);
}

void AnimationEngine::shake(lv_obj_t *obj, uint32_t duration, uint32_t delay) {
    int16_t x = lv_obj_get_x(obj);
    uint32_t d = duration / 4;
    animate(obj, anim_set_x_cb, x, x - 10, d, EASE_LINEAR, delay);
    animate(obj, anim_set_x_cb, x - 10, x + 10, d, EASE_LINEAR, delay + d);
    animate(obj, anim_set_x_cb, x + 10, x - 5, d, EASE_LINEAR, delay + d * 2);
    animate(obj, anim_set_x_cb, x - 5, x, d, EASE_OUT_QUAD, delay + d * 3);
}

void AnimationEngine::pulse(lv_obj_t *obj, uint32_t duration, uint32_t delay) {
    animate(obj, anim_set_scale_cb, 256, 280, duration / 2, EASE_OUT_QUAD, delay);
    animate(obj, anim_set_scale_cb, 280, 256, duration / 2, EASE_IN_QUAD, delay + duration / 2);
}

void AnimationEngine::rotate(lv_obj_t *obj, int16_t angle, uint32_t duration, uint32_t delay) {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 0, angle);
    lv_anim_set_time(&a, duration);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_exec_cb(&a, [](void *var, int32_t v) {
        lv_obj_set_style_transform_angle((lv_obj_t *)var, v * 10, 0);
    });
    lv_anim_start(&a);
}

void AnimationEngine::animate(
    lv_obj_t *obj, lv_anim_exec_xcb_t exec_cb, int32_t start, int32_t end, uint32_t duration,
    EasingType easing, uint32_t delay
) {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, start, end);
    lv_anim_set_time(&a, duration);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_path_cb(&a, getEasingPath(easing));
    lv_anim_start(&a);
}

void AnimationEngine::startSequence() {
    _sequenceCount = 0;
    _sequenceDelay = 0;
}

void AnimationEngine::addToSequence(
    lv_obj_t *obj, lv_anim_exec_xcb_t exec_cb, int32_t start, int32_t end, uint32_t duration,
    EasingType easing
) {
    if (_sequenceCount >= 8) return;

    lv_anim_t *a = &_sequenceAnims[_sequenceCount++];
    lv_anim_init(a);
    lv_anim_set_var(a, obj);
    lv_anim_set_values(a, start, end);
    lv_anim_set_time(a, duration);
    lv_anim_set_exec_cb(a, exec_cb);
    lv_anim_set_path_cb(a, getEasingPath(easing));
}

void AnimationEngine::playSequence(bool parallel) {
    uint32_t delay = 0;

    for (uint8_t i = 0; i < _sequenceCount; i++) {
        lv_anim_set_delay(&_sequenceAnims[i], delay);
        lv_anim_start(&_sequenceAnims[i]);

        if (!parallel) { delay += lv_anim_get_time(&_sequenceAnims[i]); }
    }
}

void AnimationEngine::pauseAll() {
    // LVGL não tem pause global, seria necessário tracking individual
}

void AnimationEngine::resumeAll() {
    // LVGL não tem resume global
}

void AnimationEngine::stopAll() { lv_anim_del_all(); }

lv_anim_path_cb_t AnimationEngine::getEasingPath(EasingType type) {
    switch (type) {
        case EASE_LINEAR: return lv_anim_path_linear;
        case EASE_IN_QUAD: return lv_anim_path_ease_in;
        case EASE_OUT_QUAD: return lv_anim_path_ease_out;
        case EASE_IN_OUT_QUAD: return lv_anim_path_ease_in_out;
        case EASE_OUT_BACK: return lv_anim_path_overshoot;
        case EASE_BOUNCE: return lv_anim_path_bounce;
        default: return lv_anim_path_ease_out;
    }
}
