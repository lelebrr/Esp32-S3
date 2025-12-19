/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Gesture Handler (Sistema de Gestos)
 * Reconhecimento de gestos touch para navegação
 * ═══════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <Arduino.h>
#include <debug_log.h>
#include <lvgl.h>


// Tipos de gestos
enum GestureType : uint8_t {
    GESTURE_NONE = 0,
    GESTURE_TAP,           // Toque simples
    GESTURE_DOUBLE_TAP,    // Toque duplo
    GESTURE_LONG_PRESS,    // Toque longo
    GESTURE_SWIPE_UP,      // Deslizar para cima
    GESTURE_SWIPE_DOWN,    // Deslizar para baixo
    GESTURE_SWIPE_LEFT,    // Deslizar para esquerda
    GESTURE_SWIPE_RIGHT,   // Deslizar para direita
    GESTURE_PINCH_IN,      // Pinch (zoom out)
    GESTURE_PINCH_OUT,     // Pinch (zoom in)
    GESTURE_TWO_FINGER_TAP // Dois dedos
};

// Callback para gestos
typedef void (*GestureCallback)(GestureType gesture, int16_t x, int16_t y);

class GestureHandler {
public:
    GestureHandler();

    void begin();
    void update();

    // Registrar callback
    void setCallback(GestureCallback callback);

    // Configurações
    void setSwipeThreshold(uint16_t pixels);
    void setLongPressTime(uint32_t ms);
    void setDoubleTapTime(uint32_t ms);

    // Último gesto detectado
    GestureType getLastGesture() const { return _lastGesture; }

    // Estado do touch
    bool isTouching() const { return _touching; }
    int16_t getTouchX() const { return _currentX; }
    int16_t getTouchY() const { return _currentY; }

private:
    GestureCallback _callback;
    GestureType _lastGesture;

    bool _touching;
    bool _wasTouching;

    int16_t _startX, _startY;
    int16_t _currentX, _currentY;
    uint32_t _touchStart;
    uint32_t _lastTap;

    uint16_t _swipeThreshold;
    uint32_t _longPressTime;
    uint32_t _doubleTapTime;

    void detectGesture();
    void triggerGesture(GestureType gesture);
};

extern GestureHandler gesture_handler;

// ═══════════════════════════════════════════════════════════════════════════
// IMPLEMENTAÇÃO
// ═══════════════════════════════════════════════════════════════════════════

GestureHandler gesture_handler;

GestureHandler::GestureHandler() {
    _callback = nullptr;
    _lastGesture = GESTURE_NONE;
    _touching = false;
    _wasTouching = false;
    _startX = _startY = 0;
    _currentX = _currentY = 0;
    _touchStart = 0;
    _lastTap = 0;

    // Defaults
    _swipeThreshold = 50;
    _longPressTime = 700;
    _doubleTapTime = 300;
}

void GestureHandler::begin() { LOG_INFO("Gesture Handler inicializado"); }

void GestureHandler::update() {
    // Lê estado do touch via LVGL
    lv_indev_t *indev = lv_indev_get_next(NULL);
    if (!indev) return;

    lv_point_t point;
    lv_indev_get_point(indev, &point);
    lv_indev_state_t state = indev->proc.state;

    _wasTouching = _touching;
    _touching = (state == LV_INDEV_STATE_PRESSED);
    _currentX = point.x;
    _currentY = point.y;

    if (_touching && !_wasTouching) {
        // Touch iniciado
        _startX = _currentX;
        _startY = _currentY;
        _touchStart = millis();
    } else if (!_touching && _wasTouching) {
        // Touch finalizado
        detectGesture();
    } else if (_touching) {
        // Touch em andamento - verifica long press
        if (millis() - _touchStart > _longPressTime) {
            int16_t dx = abs(_currentX - _startX);
            int16_t dy = abs(_currentY - _startY);

            // Só é long press se não moveu muito
            if (dx < 20 && dy < 20) {
                triggerGesture(GESTURE_LONG_PRESS);
                _touchStart = millis() + 1000; // Evita repetição
            }
        }
    }
}

void GestureHandler::detectGesture() {
    uint32_t duration = millis() - _touchStart;
    int16_t dx = _currentX - _startX;
    int16_t dy = _currentY - _startY;
    int16_t absDx = abs(dx);
    int16_t absDy = abs(dy);

    // Swipe
    if (absDx > _swipeThreshold || absDy > _swipeThreshold) {
        if (absDx > absDy) {
            // Horizontal
            triggerGesture(dx > 0 ? GESTURE_SWIPE_RIGHT : GESTURE_SWIPE_LEFT);
        } else {
            // Vertical
            triggerGesture(dy > 0 ? GESTURE_SWIPE_DOWN : GESTURE_SWIPE_UP);
        }
        return;
    }

    // Tap
    if (duration < 300 && absDx < 20 && absDy < 20) {
        // Verifica double tap
        if (millis() - _lastTap < _doubleTapTime) {
            triggerGesture(GESTURE_DOUBLE_TAP);
            _lastTap = 0;
        } else {
            _lastTap = millis();
            triggerGesture(GESTURE_TAP);
        }
    }
}

void GestureHandler::triggerGesture(GestureType gesture) {
    _lastGesture = gesture;

    const char *names[] = {
        "NONE",
        "TAP",
        "DOUBLE_TAP",
        "LONG_PRESS",
        "SWIPE_UP",
        "SWIPE_DOWN",
        "SWIPE_LEFT",
        "SWIPE_RIGHT",
        "PINCH_IN",
        "PINCH_OUT",
        "TWO_FINGER_TAP"
    };
    LOG_VERBOSE("Gesture Detectado: %s em (%d, %d)", names[gesture], _currentX, _currentY);

    if (_callback) { _callback(gesture, _currentX, _currentY); }
}

void GestureHandler::setCallback(GestureCallback callback) { _callback = callback; }

void GestureHandler::setSwipeThreshold(uint16_t pixels) { _swipeThreshold = constrain(pixels, 20, 150); }

void GestureHandler::setLongPressTime(uint32_t ms) { _longPressTime = constrain(ms, 300, 2000); }

void GestureHandler::setDoubleTapTime(uint32_t ms) { _doubleTapTime = constrain(ms, 100, 500); }
