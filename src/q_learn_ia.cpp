/**
 * @file q_learn_ia.cpp
 * @brief Q-Learning AI for Adaptive Attack Selection
 *
 * Implements basic Q-Learning algorithm for intelligent attack
 * selection based on environmental state (battery, time, WiFi activity).
 * Uses PSRAM for Q-table storage.
 *
 * @author Monster S3 Team
 * @date 2025-12-21
 */

#include "q_learn_ia.h"
#include "attacks_manager.h"
#include <WiFi.h>

#define ACTIONS 6 // None, BLE, WiFi, NFC, SubGHz, IR
#define STATES 24 // Ex: 4 Battery Levels * 2 Time of Day intervals (Day/Night) * 3 Activity Levels
#define INVALID_MAX 9999.0f

float *q_table = nullptr;
bool ai_initialized = false;

// Mapeamento de Ações
AttackType action_map[ACTIONS] = {
    ATTACK_NONE,
    ATTACK_BLE_SPAM,
    ATTACK_WIFI_DEAUTH,
    ATTACK_NFC_FAULT,
    ATTACK_RF_GHOST_REPLAY,
    ATTACK_IR_BRUTE
};

void setup_q_learn() {
    // Aloca na PSRAM
    size_t size = STATES * ACTIONS * sizeof(float);
    q_table = (float *)ps_malloc(size);

    if (q_table == NULL) {
        Serial.println("[AI] FALHA PSRAM AI. Usando Heap.");
        q_table = (float *)malloc(size);
    }

    if (q_table) {
        // Init com valores base (otimismo)
        for (int i = 0; i < STATES * ACTIONS; i++) q_table[i] = 0.5;
        ai_initialized = true;
        Serial.println("[AI] Q-Table Inicializada.");
    }
}

// Obtém estado do ambiente (Simplificado para Demo)
// Estado = (Bateria 0-3) + (Hora 0-1) * 4 + (WiFi Nets 0-2) * 8
int get_env_state() {
    // 1. Battery Level (0-3) - Mocked for now, assumes analogRead or global var available
    int bat_level = 3; // Full

    // 2. Time (Day/Night)
    int time_state = 0; // Day

    // 3. Environment Activity (Scan is expensive, using RSSI heuristic if connected)
    int env_activity = 0; // Low
    if (WiFi.status() == WL_CONNECTED) env_activity = 1;

    int state = bat_level + (time_state * 4) + (env_activity * 8);
    if (state >= STATES) state = STATES - 1;
    return state;
}

int q_choose_action(int state) {
    if (!ai_initialized) return 0;

    // Epsilon-greedy (10% exploração - reduzirá com o tempo idealmente)
    if (random(100) < 10) { return random(ACTIONS); }

    int best_action = 0;
    float max_val = -INVALID_MAX;
    for (int a = 0; a < ACTIONS; a++) {
        float val = q_table[state * ACTIONS + a];
        if (val > max_val) {
            max_val = val;
            best_action = a;
        }
    }
    return best_action;
}

void q_update(int state, int action, float reward) {
    if (!ai_initialized) return;

    float alpha = 0.1; // Learning Rate
    float gamma = 0.9; // Discount Factor

    float current = q_table[state * ACTIONS + action];

    // Estimate optimal future value (Max Q for next state)
    // For simplicity, we assume next state is similar or we re-read.
    // Ideally we pass next_state. Let's re-read env.
    int next_state = get_env_state();

    float max_next = -9999;
    for (int a = 0; a < ACTIONS; a++) {
        if (q_table[next_state * ACTIONS + a] > max_next) max_next = q_table[next_state * ACTIONS + a];
    }

    q_table[state * ACTIONS + action] = current + alpha * (reward + gamma * max_next - current);

    Serial.printf(
        "[AI] Update: State %d Action %d Reward %.2f NewQ %.2f\n",
        state,
        action,
        reward,
        q_table[state * ACTIONS + action]
    );
}

// Função Principal chamada pelo Loop
void ai_loop_step() {
    if (!ai_initialized) return;

    static unsigned long last_step = 0;
    if (millis() - last_step > 30000) { // Toma decisão a cada 30s
        last_step = millis();

        int state = get_env_state();
        int action_idx = q_choose_action(state);
        AttackType attack = action_map[action_idx];

        Serial.printf("[AI] Decisao: State %d -> Action %d (Attack %d)\n", state, action_idx, attack);

        // Executa
        if (attack != ATTACK_NONE) {
            attacks_start(attack);
            // Reward será calculado posterioremente basedo no feedback (ex: handshake capturado)
            // Por enquanto, aplicamos um reward de "atividade"
            q_update(state, action_idx, 0.1);
        } else {
            attacks_stop();
            q_update(state, action_idx, 0.05); // Pequeno reward por economizar energia
        }
    }
}
