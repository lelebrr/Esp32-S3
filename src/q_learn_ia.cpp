/**
 * @file q_learn_ia.cpp
 * @brief Q-Learning AI for Adaptive Attack Selection
 *
 * Implements Q-Learning algorithm for intelligent attack selection based on
 * environmental state (battery, time, WiFi activity). Uses PSRAM for Q-table
 * storage and persists to SD card.
 *
 * Features:
 * - Epsilon-greedy exploration (10% exploration rate)
 * - Q-Table persistence to /ai/q_table.bin
 * - Combat mode for autonomous attack execution
 * - TTS integration for voice feedback
 *
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#include "q_learn_ia.h"
#include "attacks_manager.h"
#include "tts_espeak.h"
#include <WiFi.h>
#include <SD.h>

// ============================================================================
// CONFIGURATION
// ============================================================================

#define ACTIONS 6   // None, BLE, WiFi, NFC, SubGHz, IR
#define STATES 24   // 4 Battery * 2 Time * 3 Activity
#define INVALID_MAX 9999.0f

#define Q_TABLE_PATH "/ai/q_table.bin"
#define HISTORY_PATH "/ai/history.json"
#define SAVE_INTERVAL 5  // Save every N updates

// ============================================================================
// STATE
// ============================================================================

static float *q_table = nullptr;
static bool ai_initialized = false;
static bool combat_mode = false;
static int update_count = 0;

// Action mapping
static AttackType action_map[ACTIONS] = {
    ATTACK_NONE,
    ATTACK_BLE_SPAM,
    ATTACK_WIFI_DEAUTH,
    ATTACK_NFC_FAULT,
    ATTACK_RF_GHOST_REPLAY,
    ATTACK_IR_BRUTE
};

static const char* action_names[ACTIONS] = {
    "none",
    "BLE Spam",
    "WiFi Deauth", 
    "NFC Fault",
    "SubGHz Replay",
    "IR Brute"
};

// ============================================================================
// PERSISTENCE
// ============================================================================

bool q_save_table() {
    if (!q_table || !ai_initialized) return false;
    
    // Ensure directory exists
    if (!SD.exists("/ai")) {
        SD.mkdir("/ai");
    }
    
    File file = SD.open(Q_TABLE_PATH, FILE_WRITE);
    if (!file) {
        Serial.println("[AI] Failed to open q_table.bin for writing");
        return false;
    }
    
    size_t size = STATES * ACTIONS * sizeof(float);
    size_t written = file.write((uint8_t*)q_table, size);
    file.close();
    
    if (written == size) {
        Serial.printf("[AI] Q-Table saved to SD (%d bytes)\n", written);
        return true;
    } else {
        Serial.printf("[AI] Q-Table save failed: %d/%d bytes\n", written, size);
        return false;
    }
}

bool q_load_table() {
    if (!q_table) return false;
    
    if (!SD.exists(Q_TABLE_PATH)) {
        Serial.println("[AI] No saved Q-Table found, using defaults");
        return false;
    }
    
    File file = SD.open(Q_TABLE_PATH, FILE_READ);
    if (!file) {
        Serial.println("[AI] Failed to open q_table.bin for reading");
        return false;
    }
    
    size_t size = STATES * ACTIONS * sizeof(float);
    size_t read = file.read((uint8_t*)q_table, size);
    file.close();
    
    if (read == size) {
        Serial.printf("[AI] Q-Table loaded from SD (%d bytes)\n", read);
        return true;
    } else {
        Serial.printf("[AI] Q-Table load failed: %d/%d bytes\n", read, size);
        // Reset to defaults
        for (int i = 0; i < STATES * ACTIONS; i++) q_table[i] = 0.5f;
        return false;
    }
}

static void save_history_entry(int state, int action, float reward) {
    File file = SD.open(HISTORY_PATH, FILE_APPEND);
    if (file) {
        file.printf("{\"ts\":%lu,\"s\":%d,\"a\":%d,\"r\":%.2f}\n",
                    millis(), state, action, reward);
        file.close();
    }
}

// ============================================================================
// Q-LEARNING CORE
// ============================================================================

void setup_q_learn() {
    if (ai_initialized) return;
    
    // Allocate in PSRAM if available
    size_t size = STATES * ACTIONS * sizeof(float);
    q_table = (float *)ps_malloc(size);

    if (q_table == NULL) {
        Serial.println("[AI] PSRAM allocation failed, using heap");
        q_table = (float *)malloc(size);
    }

    if (q_table) {
        // Initialize with optimistic values
        for (int i = 0; i < STATES * ACTIONS; i++) {
            q_table[i] = 0.5f;
        }
        
        // Try to load saved Q-Table from SD
        if (SD.begin()) {
            q_load_table();
        }
        
        ai_initialized = true;
        update_count = 0;
        Serial.println("[AI] Q-Learning initialized");
        Serial.printf("[AI] Q-Table: %d states x %d actions = %d entries\n", 
                      STATES, ACTIONS, STATES * ACTIONS);
    } else {
        Serial.println("[AI] FATAL: Q-Table allocation failed!");
    }
}

int get_env_state() {
    // 1. Battery Level (0-3) - Simplified
    int bat_level = 3; // Full (TODO: read from ADC)

    // 2. Time of Day (0=Day, 1=Night) - Simplified
    int time_state = 0; // Day

    // 3. Environment Activity (0-2)
    int env_activity = 0; // Low
    if (WiFi.status() == WL_CONNECTED) env_activity = 1;
    
    // Check for nearby networks
    int n = WiFi.scanComplete();
    if (n > 5) env_activity = 2;

    int state = bat_level + (time_state * 4) + (env_activity * 8);
    if (state >= STATES) state = STATES - 1;
    return state;
}

int q_choose_action(int state) {
    if (!ai_initialized) return 0;

    // Epsilon-greedy: 10% exploration
    if (random(100) < 10) {
        int action = random(ACTIONS);
        Serial.printf("[AI] Exploring: random action %d\n", action);
        return action;
    }

    // Exploitation: choose best action
    int best_action = 0;
    float max_val = -INVALID_MAX;
    for (int a = 0; a < ACTIONS; a++) {
        float val = q_table[state * ACTIONS + a];
        if (val > max_val) {
            max_val = val;
            best_action = a;
        }
    }
    
    Serial.printf("[AI] Exploiting: best action %d (Q=%.3f)\n", best_action, max_val);
    return best_action;
}

void q_update(int state, int action, float reward) {
    if (!ai_initialized) return;

    float alpha = 0.1f;  // Learning rate
    float gamma = 0.9f;  // Discount factor

    float current = q_table[state * ACTIONS + action];

    // Get next state
    int next_state = get_env_state();

    // Find max Q for next state
    float max_next = -INVALID_MAX;
    for (int a = 0; a < ACTIONS; a++) {
        if (q_table[next_state * ACTIONS + a] > max_next) {
            max_next = q_table[next_state * ACTIONS + a];
        }
    }

    // Bellman equation update
    float new_q = current + alpha * (reward + gamma * max_next - current);
    q_table[state * ACTIONS + action] = new_q;

    Serial.printf("[AI] Update: S%d A%d R%.2f -> Q%.3f\n",
                  state, action, reward, new_q);

    // Save history
    save_history_entry(state, action, reward);
    
    // Periodic save to SD
    update_count++;
    if (update_count >= SAVE_INTERVAL) {
        q_save_table();
        update_count = 0;
    }
}

// ============================================================================
// COMBAT MODE
// ============================================================================

void ai_set_combat_mode(bool enabled) {
    combat_mode = enabled;
    Serial.printf("[AI] Combat mode: %s\n", enabled ? "ENABLED" : "DISABLED");
    
    if (enabled) {
        tts_speak("ia_escolheu");
    }
}

bool ai_get_combat_mode() {
    return combat_mode;
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void ai_loop_step() {
    if (!ai_initialized) return;
    if (!combat_mode) return;  // Only run in combat mode

    static unsigned long last_step = 0;
    
    // Take decision every 30 seconds
    if (millis() - last_step < 30000) return;
    last_step = millis();

    int state = get_env_state();
    int action_idx = q_choose_action(state);
    AttackType attack = action_map[action_idx];

    Serial.printf("[AI] Decision: State %d -> Action %d (%s)\n", 
                  state, action_idx, action_names[action_idx]);

    // Execute attack
    if (attack != ATTACK_NONE) {
        // Voice feedback before attack
        if (attack == ATTACK_WIFI_DEAUTH) {
            tts_speak("vou_deauth");
        } else if (attack == ATTACK_BLE_SPAM) {
            tts_speak("ble_spamando");
        } else {
            tts_speak("ataque_iniciado");
        }
        
        vTaskDelay(500 / portTICK_PERIOD_MS);  // Wait for TTS
        
        attacks_start(attack);
        
        // Initial reward for activity
        q_update(state, action_idx, 0.1f);
    } else {
        attacks_stop();
        // Small reward for energy saving
        q_update(state, action_idx, 0.05f);
    }
}

// ============================================================================
// FEEDBACK API
// ============================================================================

void ai_give_reward(float reward) {
    if (!ai_initialized) return;
    
    int state = get_env_state();
    AttackType current = attacks_get_current();
    
    // Find action index for current attack
    int action_idx = 0;
    for (int i = 0; i < ACTIONS; i++) {
        if (action_map[i] == current) {
            action_idx = i;
            break;
        }
    }
    
    q_update(state, action_idx, reward);
    Serial.printf("[AI] External reward: %.2f for action %d\n", reward, action_idx);
}

void ai_report_success() {
    ai_give_reward(1.0f);
    Serial.println("[AI] Attack SUCCESS reported (+1.0)");
}

void ai_report_failure() {
    ai_give_reward(-1.0f);
    Serial.println("[AI] Attack FAILURE reported (-1.0)");
}
