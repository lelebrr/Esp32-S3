/**
 * @file burn_in_protection.cpp
 * @brief Implementação da proteção contra burn-in
 */

#include "burn_in_protection.h"

BurnInProtection burnInProtection;

BurnInProtection::BurnInProtection()
    : _lastShiftTime(0), _currentOffsetX(0), _currentOffsetY(0), _directionX(1), _directionY(1),
      _objectCount(0) {
    _config.enabled = true;
    _config.interval_s = 30;
    _config.max_offset = 2;
    _config.apply_to_all = false;

    for (int i = 0; i < MAX_PROTECTED_OBJECTS; i++) {
        _protectedObjects[i] = nullptr;
        _originalX[i] = 0;
        _originalY[i] = 0;
    }
}

void BurnInProtection::begin() { _lastShiftTime = millis(); }

void BurnInProtection::update() {
    if (!_config.enabled) return;

    uint32_t now = millis();
    if (now - _lastShiftTime >= (_config.interval_s * 1000)) {
        _lastShiftTime = now;
        applyShift();
    }
}

void BurnInProtection::setEnabled(bool enable) {
    _config.enabled = enable;
    if (!enable) { resetPositions(); }
}

void BurnInProtection::setInterval(uint16_t seconds) {
    if (seconds < 10) seconds = 10;
    if (seconds > 120) seconds = 120;
    _config.interval_s = seconds;
}

void BurnInProtection::setMaxOffset(int8_t pixels) {
    if (pixels < 1) pixels = 1;
    if (pixels > 5) pixels = 5;
    _config.max_offset = pixels;
}

void BurnInProtection::registerObject(lv_obj_t *obj) {
    if (!obj || _objectCount >= MAX_PROTECTED_OBJECTS) return;

    // Verifica se já está registrado
    for (int i = 0; i < _objectCount; i++) {
        if (_protectedObjects[i] == obj) return;
    }

    // Adiciona objeto
    _protectedObjects[_objectCount] = obj;
    _originalX[_objectCount] = lv_obj_get_x(obj);
    _originalY[_objectCount] = lv_obj_get_y(obj);
    _objectCount++;
}

void BurnInProtection::unregisterObject(lv_obj_t *obj) {
    for (int i = 0; i < _objectCount; i++) {
        if (_protectedObjects[i] == obj) {
            // Restaura posição original
            lv_obj_set_pos(obj, _originalX[i], _originalY[i]);

            // Remove do array (shift elementos)
            for (int j = i; j < _objectCount - 1; j++) {
                _protectedObjects[j] = _protectedObjects[j + 1];
                _originalX[j] = _originalX[j + 1];
                _originalY[j] = _originalY[j + 1];
            }
            _objectCount--;
            break;
        }
    }
}

void BurnInProtection::clearObjects() {
    resetPositions();
    for (int i = 0; i < MAX_PROTECTED_OBJECTS; i++) { _protectedObjects[i] = nullptr; }
    _objectCount = 0;
}

void BurnInProtection::setConfig(const BurnInConfig &config) { _config = config; }

void BurnInProtection::applyShift() {
    // Calcula novo offset
    _currentOffsetX += _directionX;
    _currentOffsetY += _directionY;

    // Inverte direção se atingir limite
    if (_currentOffsetX >= _config.max_offset || _currentOffsetX <= -_config.max_offset) {
        _directionX = -_directionX;
    }
    if (_currentOffsetY >= _config.max_offset || _currentOffsetY <= -_config.max_offset) {
        _directionY = -_directionY;
    }

    // Aplica offset a todos os objetos registrados
    for (int i = 0; i < _objectCount; i++) {
        if (_protectedObjects[i]) {
            lv_obj_set_pos(
                _protectedObjects[i], _originalX[i] + _currentOffsetX, _originalY[i] + _currentOffsetY
            );
        }
    }

    // Log para debug (opcional)
    // LOG_DEBUG("BurnIn Shift: X=%d, Y=%d", _currentOffsetX,
    // _currentOffsetY);
}

void BurnInProtection::resetPositions() {
    _currentOffsetX = 0;
    _currentOffsetY = 0;

    for (int i = 0; i < _objectCount; i++) {
        if (_protectedObjects[i]) { lv_obj_set_pos(_protectedObjects[i], _originalX[i], _originalY[i]); }
    }
}
