/**
 * @file q_learn_ia.cpp
 * Monster S3 Firmware - Q-Learning AI Engine
 * 
 * Features:
 * - Q-table in PSRAM with SD persistence
 * - User feedback via physical buttons (success/fail)
 * - Epsilon-greedy with decay
 * - Real-time state from sensors (battery, WiFi, RTC)
 */

#include "q_learn_ia.h"
#include "attacks_manager.h"
#include "pin_config.h"
#include "tts_espeak.h"
#include "config_manager.h"
#include <WiFi.h>
#include <SD.h>

// ============================================================================
// CONSTANTS
// ============================================================================
#define ACTIONS 6       // None, BLE, WiFi, NFC, SubGHz, IR
#define STATES 48       // Expanded: 4 Battery * 2 Time * 3 Activity * 2 RSSI
#define Q_TABLE_PATH "/ai/qtable.bin"
#define Q_LOG_PATH "/ai/decisions.log"

// Learning parameters (Default constants replaced by Config)
static const float EPSILON_MIN = 0.01f;
static const float EPSILON_DECAY = 0.995f;

// ============================================================================
// STATE
// ============================================================================
static float* q_table = nullptr;
static bool ai_initialized = false;
static float epsilon; // Initialized from config or EPSILON_START
static uint32_t decision_count = 0;
static int last_state = 0;
static int last_action = 0;

// Action mapping
static AttackType action_map[ACTIONS] = {
    ATTACK_NONE,
    ATTACK_BLE_SPAM,
    ATTACK_WIFI_DEAUTH,
    ATTACK_NFC_FAULT,
    ATTACK_RF_GHOST_REPLAY,
    ATTACK_IR_BRUTE
};

// ============================================================================
// Q-TABLE PERSISTENCE
// ============================================================================

/**
 * @brief Save Q-table to SD card
 */
static bool save_q_table() {
    if (!q_table || !ai_initialized) return false;
    
    // Ensure directory exists
    if (!SD.exists("/ai")) {
        SD.mkdir("/ai");
    }
    
    File file = SD.open(Q_TABLE_PATH, FILE_WRITE);
    if (!file) {
        Serial.println("[AI] Failed to save Q-table");
        return false;
    }
    
    // Write header
    uint32_t magic = 0x51544142; // "QTAB"
    uint32_t states = STATES;
    uint32_t actions = ACTIONS;
    file.write((uint8_t*)&magic, 4);
    file.write((uint8_t*)&states, 4);
    file.write((uint8_t*)&actions, 4);
    file.write((uint8_t*)&epsilon, sizeof(float));
    file.write((uint8_t*)&decision_count, 4);
    
    // Write Q-values
    size_t size = STATES * ACTIONS * sizeof(float);
    file.write((uint8_t*)q_table, size);
    
    file.close();
    Serial.printf("[AI] Q-table saved (%d decisions, epsilon=%.3f)\n", decision_count, epsilon);
    return true;
}

/**
 * @brief Load Q-table from SD card
 */
static bool load_q_table() {
    if (!q_table) return false;
    
    if (!SD.exists(Q_TABLE_PATH)) {
        Serial.println("[AI] No saved Q-table found, starting fresh");
        return false;
    }
    
    File file = SD.open(Q_TABLE_PATH, FILE_READ);
    if (!file) {
        Serial.println("[AI] Failed to open Q-table");
        return false;
    }
    
    // Verify header
    uint32_t magic, states, actions;
    file.read((uint8_t*)&magic, 4);
    file.read((uint8_t*)&states, 4);
    file.read((uint8_t*)&actions, 4);
    
    if (magic != 0x51544142 || states != STATES || actions != ACTIONS) {
        Serial.println("[AI] Q-table format mismatch, resetting");
        file.close();
        return false;
    }
    
    // Read saved epsilon and decision count
    file.read((uint8_t*)&epsilon, sizeof(float));
    file.read((uint8_t*)&decision_count, 4);
    
    // Read Q-values
    size_t size = STATES * ACTIONS * sizeof(float);
    file.read((uint8_t*)q_table, size);
    
    file.close();
    Serial.printf("[AI] Q-table loaded (%d decisions, epsilon=%.3f)\n", decision_count, epsilon);
    return true;
}

/**
 * @brief Log decision to SD for analysis
 */
static void log_decision(int state, int action, float reward, float q_value) {
    File file = SD.open(Q_LOG_PATH, FILE_APPEND);
    if (file) {
        char log[64];
        snprintf(log, sizeof(log), "%lu,S%d,A%d,R%.2f,Q%.2f\n", 
                 millis(), state, action, reward, q_value);
        file.print(log);
        file.close();
    }
}

// ============================================================================
// STATE ENCODING
// ============================================================================

/**
 * @brief Get current environment state
 * State = Battery(0-3) + Time(0-1)*4 + Activity(0-2)*8 + RSSI(0-1)*24
 */
static int get_env_state() {
    // 1. Battery Level (0-3) - Mock for now, TODO: add ADC battery monitor
    int bat_level = 3; // Full
    
    // 2. Time of day (0=day 6AM-6PM, 1=night)
    // TODO: Use DS3231 RTC when available
    int time_state = 0;
    
    // 3. Environment activity based on WiFi networks visible
    int env_activity = 0; // Low
    int16_t rssi = WiFi.RSSI();
    if (WiFi.status() == WL_CONNECTED) {
        env_activity = (rssi > -60) ? 2 : 1; // Strong/Weak connection
    }
    
    // 4. RSSI quality (0=weak, 1=strong)
    int rssi_state = (rssi > -70) ? 1 : 0;
    
    int state = bat_level + (time_state * 4) + (env_activity * 8) + (rssi_state * 24);
    if (state >= STATES) state = STATES - 1;
    
    return state;
}

// ============================================================================
// PUBLIC API
// ============================================================================

/**
 * @brief Initialize Q-learning system
 */
void setup_q_learn() {
    // Allocate Q-table in PSRAM
    size_t size = STATES * ACTIONS * sizeof(float);
    q_table = (float*) ps_malloc(size);
    
    if (q_table == NULL) {
        Serial.println("[AI] PSRAM allocation failed, using heap");
        q_table = (float*) malloc(size);
    }
    
    if (!q_table) {
        Serial.println("[AI] FATAL: Cannot allocate Q-table!");
        return;
    }
    
    const DeviceConfig* cfg = config_get();
    
    // Initialize with optimistic values
    for (int i = 0; i < STATES * ACTIONS; i++) {
        q_table[i] = 0.5f;
    }
    
    // Try to load saved Q-table
    if (!load_q_table()) {
        epsilon = cfg->ai_epsilon > 0 ? cfg->ai_epsilon : 0.15f;
        decision_count = 0;
    }
    
    ai_initialized = true;
    Serial.printf("[AI] Q-Learning initialized (PSRAM: %d bytes, %d states, %d actions)\n", 
                  size, STATES, ACTIONS);
}

/**
 * @brief Choose best action for current state (epsilon-greedy)
 */
int q_choose_action(int state) {
    if (!ai_initialized) return 0;
    
    // Epsilon-greedy exploration
    if ((random(1000) / 1000.0f) < epsilon) {
        return random(ACTIONS);
    }
    
    // Exploit: choose best known action
    int best_action = 0;
    float max_val = -9999.0f;
    
    for (int a = 0; a < ACTIONS; a++) {
        float val = q_table[state * ACTIONS + a];
        if (val > max_val) {
            max_val = val;
            best_action = a;
        }
    }
    
    return best_action;
}

/**
 * @brief Update Q-value after receiving reward
 */
void q_update(int state, int action, float reward) {
    if (!ai_initialized) return;
    
    // Get config for learning parameters
    const DeviceConfig* cfg = config_get();
    
    float current = q_table[state * ACTIONS + action];
    
    // Get max Q for next state
    int next_state = get_env_state();
    float max_next = -9999.0f;
    for (int a = 0; a < ACTIONS; a++) {
        float val = q_table[next_state * ACTIONS + a];
        if (val > max_next) max_next = val;
    }
    
    // Q-learning update rule with configurable Alpha/Gamma
    float alpha = cfg->ai_learning_rate > 0 ? cfg->ai_learning_rate : 0.1f;
    float gamma = cfg->ai_discount_factor > 0 ? cfg->ai_discount_factor : 0.9f;
    
    float new_q = current + alpha * (reward + gamma * max_next - current);
    q_table[state * ACTIONS + action] = new_q;
    
    // Decay epsilon
    if (epsilon > EPSILON_MIN) {
        epsilon *= EPSILON_DECAY;
    }
    
    // Increment decision count
    decision_count++;
    
    // Log decision
    log_decision(state, action, reward, new_q);
    
    // Auto-save every 50 decisions
    if (decision_count % 50 == 0) {
        save_q_table();
    }
    
    Serial.printf("[AI] Update: S%d A%d R%.2f Q%.2f->%.2f (eps=%.3f)\n", 
                  state, action, reward, current, new_q, epsilon);
}

/**
 * @brief User feedback: attack was successful (+1.0 reward)
 */
void ai_feedback_success() {
    if (!ai_initialized) return;
    
    q_update(last_state, last_action, 1.0f);
    speak(TTS_SUCCESS);
    Serial.println("[AI] User feedback: SUCCESS (+1.0)");
}

/**
 * @brief User feedback: attack failed (-1.0 reward)
 */
void ai_feedback_fail() {
    if (!ai_initialized) return;
    
    q_update(last_state, last_action, -1.0f);
    speak(TTS_FAIL);
    Serial.println("[AI] User feedback: FAIL (-1.0)");
}

/**
 * @brief Force save Q-table to SD
 */
void ai_save() {
    save_q_table();
}

/**
 * @brief Main AI loop - makes decisions periodically
 */
void ai_loop_step() {
    if (!ai_initialized) return;
    
    static unsigned long last_step = 0;
    if (millis() - last_step < 30000) return; // Decision every 30s
    last_step = millis();
    
    // Get current state
    last_state = get_env_state();
    
    // Choose action
    last_action = q_choose_action(last_state);
    AttackType attack = action_map[last_action];
    
    Serial.printf("[AI] Decision #%d: State %d -> Action %d (%s)\n", 
                  decision_count, last_state, last_action, attacks_get_name(attack));
    
    // Execute action
    if (attack != ATTACK_NONE) {
        speak(TTS_ATTACK_START);
        attacks_start(attack);
        
        // Base reward for attempting attack (user feedback adjusts later)
        q_update(last_state, last_action, 0.1f);
    } else {
        attacks_stop();
        // Small reward for power saving
        q_update(last_state, last_action, 0.05f);
    }
}

/**
 * @brief Get current AI stats
 */
void ai_get_stats(uint32_t* decisions, float* eps) {
    if (decisions) *decisions = decision_count;
    if (eps) *eps = epsilon;
}
