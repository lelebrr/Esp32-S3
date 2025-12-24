/**
 * @file gesture_actions.cpp
 * @brief Implementação de Integração Gestos → Ataques
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#include "gesture_actions.h"
#include "module_manager.h"
#include "led_driver.h"
#include "core/aggressive_sd.h"
#include <ArduinoJson.h>

// Static member initialization
bool GestureActions::_enabled = true;
AttackType GestureActions::_mappings[10] = {ATTACK_NONE};
uint32_t GestureActions::_lastGestureTime = 0;
GestureType GestureActions::_lastGesture = GESTURE_NONE;

// Debounce time (ms)
#define GESTURE_DEBOUNCE 500

// ============================================================================
// INICIALIZAÇÃO
// ============================================================================

void GestureActions::init() {
    Serial.println("[GESTURE] Initializing action system...");
    
    // Inicializa sensor
    GestureSensor::init();
    
    // Carrega mapeamentos padrão
    loadDefaultMappings();
    
    Serial.println("[GESTURE] Ready - wave to start!");
}

void GestureActions::loadDefaultMappings() {
    // Mapeamentos padrão intuitivoss
    
    // Swipe Right = WiFi Deauth (ataque ofensivo "vai embora")
    _mappings[GESTURE_RIGHT] = ATTACK_WIFI_DEAUTH;
    
    // Swipe Left = Para ataque
    _mappings[GESTURE_LEFT] = ATTACK_NONE;
    
    // Swipe Up = BLE Spam (enche de notificações "para cima")
    _mappings[GESTURE_UP] = ATTACK_BLE_SPAM;
    
    // Swipe Down = RF Jammer ("abafa" sinal)
    _mappings[GESTURE_DOWN] = ATTACK_RF_JAMMER_433;
    
    // Forward = Evil Twin (vai atrás do alvo)
    _mappings[GESTURE_FORWARD] = ATTACK_WIFI_EVIL_TWIN;
    
    // Backward = NFC Clone (puxa dados)
    _mappings[GESTURE_BACKWARD] = ATTACK_NFC_CLONE;
    
    // Clockwise = Beacon Spam (gira criando APs)
    _mappings[GESTURE_CLOCKWISE] = ATTACK_WIFI_BEACON_SPAM;
    
    // Counter-clockwise = IR TV Off
    _mappings[GESTURE_ANTICLOCKWISE] = ATTACK_IR_TV_OFF;
    
    // Wave = Toggle ataque atual
    _mappings[GESTURE_WAVE] = ATTACK_NONE;  // Special: toggle
    
    Serial.println("[GESTURE] Default mappings loaded:");
    Serial.println("  RIGHT = WiFi Deauth");
    Serial.println("  LEFT  = Stop Attack");
    Serial.println("  UP    = BLE Spam");
    Serial.println("  DOWN  = RF Jammer 433");
    Serial.println("  FWD   = Evil Twin");
    Serial.println("  BACK  = NFC Clone");
    Serial.println("  CW    = Beacon Spam");
    Serial.println("  CCW   = IR TV Off");
}

// ============================================================================
// UPDATE LOOP
// ============================================================================

void GestureActions::update() {
    if (!_enabled) return;
    
    // Atualiza sensor
    GestureSensor::update();
    
    // Pega último gesto
    GestureType gesture = GestureSensor::getLastGesture();
    
    if (gesture == GESTURE_NONE) return;
    
    // Debounce
    uint32_t now = millis();
    if (now - _lastGestureTime < GESTURE_DEBOUNCE) return;
    if (gesture == _lastGesture && now - _lastGestureTime < GESTURE_DEBOUNCE * 2) return;
    
    _lastGestureTime = now;
    _lastGesture = gesture;
    
    // Executa ação
    executeGestureAction(gesture);
}

void GestureActions::executeGestureAction(GestureType gesture) {
    // Log gesto
    const char* gestureNames[] = {
        "NONE", "UP", "DOWN", "LEFT", "RIGHT",
        "FORWARD", "BACKWARD", "CLOCKWISE", "COUNTERCLOCKWISE", "WAVE"
    };
    
    Serial.printf("[GESTURE] Detected: %s\n", gestureNames[gesture]);
    
    // LED feedback
    LEDDriver::setAll(LED_CYAN);
    LEDDriver::show();
    delay(100);
    LEDDriver::clear();
    
    // Wave especial: toggle ataque
    if (gesture == GESTURE_WAVE) {
        if (attacks_is_running()) {
            attacks_stop();
            LEDDriver::blinkError();  // Vermelho = parou
            Serial.println("[GESTURE] WAVE: Stopped attack");
        } else {
            Serial.println("[GESTURE] WAVE: No attack to toggle");
        }
        return;
    }
    
    // Left especial: sempre para
    if (gesture == GESTURE_LEFT) {
        attacks_stop();
        LEDDriver::blinkError();
        Serial.println("[GESTURE] LEFT: Stopped attack");
        return;
    }
    
    // Pega ataque mapeado
    AttackType attack = _mappings[gesture];
    
    if (attack == ATTACK_NONE) {
        Serial.println("[GESTURE] No attack mapped");
        return;
    }
    
    // Para ataque atual se houver
    if (attacks_is_running()) {
        attacks_stop();
        delay(100);
    }
    
    // Inicia novo ataque
    Serial.printf("[GESTURE] Starting attack: %d\n", attack);
    attacks_start(attack);
    
    LEDDriver::blinkSuccess();  // Verde = iniciou
}

// ============================================================================
// CONFIGURAÇÃO
// ============================================================================

void GestureActions::setEnabled(bool enabled) {
    _enabled = enabled;
    Serial.printf("[GESTURE] Control %s\n", enabled ? "enabled" : "disabled");
}

bool GestureActions::isEnabled() {
    return _enabled;
}

void GestureActions::setMapping(GestureType gesture, AttackType attack) {
    if (gesture >= 0 && gesture < 10) {
        _mappings[gesture] = attack;
    }
}

AttackType GestureActions::getMappedAttack(GestureType gesture) {
    if (gesture >= 0 && gesture < 10) {
        return _mappings[gesture];
    }
    return ATTACK_NONE;
}

// ============================================================================
// PERSISTÊNCIA
// ============================================================================

bool GestureActions::saveMappings(const char* filename) {
    JsonDocument doc;
    
    const char* gestureNames[] = {
        "none", "up", "down", "left", "right",
        "forward", "backward", "clockwise", "counterclockwise", "wave"
    };
    
    for (int i = 0; i < 10; i++) {
        doc[gestureNames[i]] = (int)_mappings[i];
    }
    
    String json;
    serializeJson(doc, json);
    
    return AggressiveSD::writeFile(filename, json.c_str());
}

bool GestureActions::loadMappings(const char* filename) {
    String content = AggressiveSD::readFile(filename);
    
    if (content.length() == 0) return false;
    
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, content);
    
    if (error) return false;
    
    _mappings[GESTURE_UP] = (AttackType)doc["up"].as<int>();
    _mappings[GESTURE_DOWN] = (AttackType)doc["down"].as<int>();
    _mappings[GESTURE_LEFT] = (AttackType)doc["left"].as<int>();
    _mappings[GESTURE_RIGHT] = (AttackType)doc["right"].as<int>();
    _mappings[GESTURE_FORWARD] = (AttackType)doc["forward"].as<int>();
    _mappings[GESTURE_BACKWARD] = (AttackType)doc["backward"].as<int>();
    _mappings[GESTURE_CLOCKWISE] = (AttackType)doc["clockwise"].as<int>();
    _mappings[GESTURE_ANTICLOCKWISE] = (AttackType)doc["counterclockwise"].as<int>();
    _mappings[GESTURE_WAVE] = (AttackType)doc["wave"].as<int>();
    
    Serial.println("[GESTURE] Mappings loaded from SD");
    return true;
}
