/**
 * @file gesture_actions.h
 * @brief Integração Gestos → Ataques
 * 
 * Mapeia gestos do PAJ7620U2 para ações de ataque.
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#ifndef GESTURE_ACTIONS_H
#define GESTURE_ACTIONS_H

#include <Arduino.h>
#include "gesture_sensor.h"
#include "attacks_manager.h"

// Mapeamento de gestos para ataques
typedef struct {
    GestureType gesture;
    AttackType attack;
    const char* description;
} GestureMapping;

class GestureActions {
public:
    /**
     * @brief Inicializa sistema de gestos com ações
     */
    static void init();
    
    /**
     * @brief Processa gestos e dispara ataques (chamar no loop)
     */
    static void update();
    
    /**
     * @brief Habilita/desabilita controle por gestos
     */
    static void setEnabled(bool enabled);
    
    /**
     * @brief Verifica se controle por gestos está ativo
     */
    static bool isEnabled();
    
    /**
     * @brief Configura mapeamento de gesto para ataque
     * @param gesture Gesto de entrada
     * @param attack Ataque a executar (-1 = nenhum)
     */
    static void setMapping(GestureType gesture, AttackType attack);
    
    /**
     * @brief Retorna ataque mapeado para gesto
     */
    static AttackType getMappedAttack(GestureType gesture);
    
    /**
     * @brief Carrega mapeamentos padrão
     */
    static void loadDefaultMappings();
    
    /**
     * @brief Salva mapeamentos em SD
     */
    static bool saveMappings(const char* filename);
    
    /**
     * @brief Carrega mapeamentos de SD
     */
    static bool loadMappings(const char* filename);

private:
    static bool _enabled;
    static AttackType _mappings[10];  // 10 gestos possíveis
    static uint32_t _lastGestureTime;
    static GestureType _lastGesture;
    
    static void executeGestureAction(GestureType gesture);
};

#endif // GESTURE_ACTIONS_H
