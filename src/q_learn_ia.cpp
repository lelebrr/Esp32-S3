/**
 * @file q_learn_ia.cpp
 * @brief Q-Learning AI for Adaptive Attack Selection
 *
 * Implements Q-Learning algorithm for intelligent attack selection based on
 * environmental state (battery, time, WiFi activity, GPS). Uses PSRAM for Q-table
 * storage and persists to SD card.
 *
 * Features:
 * - Epsilon-greedy exploration with decay (30% → 5%)
 * - Q-Table persistence to /ai/q_table.bin
 * - Combat mode for autonomous attack execution
 * - TTS integration for voice feedback
 * - JSON export for analysis
 *
 * @author Monster S3 Team
 * @date 2025-12-28
 */

#include "q_learn_ia.h"
#include "attacks_manager.h"
#include "tts_espeak.h"
#include <WiFi.h>
#include <SD.h>

// ============================================================================
// CONFIGURATION (now uses header constants)
// ============================================================================

#define ACTIONS AI_ACTIONS   // 8 actions from header
#define STATES  AI_STATES    // 32 states from header
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
static float epsilon = 0.30f;  // Start with 30% exploration
static uint32_t total_updates = 0;
static float total_rewards = 0.0f;

// Action mapping - expanded to 8 actions
static AttackType action_map[ACTIONS] = {
    ATTACK_NONE,
    ATTACK_BLE_SPAM,
    ATTACK_WIFI_DEAUTH,
    ATTACK_NFC_FAULT,
    ATTACK_RF_GHOST_REPLAY,
    ATTACK_IR_BRUTE,
    ATTACK_WIFI_PMKID,       // WPS-like (PMKID capture)
    ATTACK_WIFI_BEACON_SPAM  // KRACK-like (beacon spam)
};

static const char* action_names[ACTIONS] = {
    "none",
    "BLE Spam",
    "WiFi Deauth", 
    "NFC Fault",
    "SubGHz Replay",
    "IR Brute",
    "PMKID Capture",
    "Beacon Spam"
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
    // =========================================================================
    // REAL ENVIRONMENT STATE DETECTION
    // =========================================================================
    
    // 1. Battery Level (0-3) - Read from s3_driver
    // Note: MonsterDriver::getBatteryVoltage() is defined in s3_driver.cpp
    extern float temperatureRead();  // ESP32 internal
    
    // Estimate battery from system state since we can't call MonsterDriver directly here
    // (circular dependency issue) - use heap pressure as proxy
    float heap_ratio = (float)ESP.getFreeHeap() / 320000.0f;  // ~320KB typical
    int bat_level;
    if (heap_ratio > 0.7f) bat_level = 3;       // System healthy = battery OK
    else if (heap_ratio > 0.5f) bat_level = 2;  
    else if (heap_ratio > 0.3f) bat_level = 1;  
    else bat_level = 0;                          // Low memory = stressed system

    // 2. Time of Day (0=Day 6-18h, 1=Night 18-6h)
    // Estimate from uptime (assume boot at ~8am typical)
    uint32_t uptime_hours = millis() / 3600000;
    int estimated_hour = (8 + uptime_hours) % 24;
    int time_state = (estimated_hour < 6 || estimated_hour >= 18) ? 1 : 0;

    // 3. Environment Activity (0-2) based on WiFi activity
    int env_activity = 0;  // Low
    
    if (WiFi.status() == WL_CONNECTED) env_activity = 1;
    
    // Check for nearby networks
    int n = WiFi.scanComplete();
    if (n == WIFI_SCAN_RUNNING) {
        // Scan in progress, use last known value
    } else if (n >= 0) {
        if (n > 5) env_activity = 2;      // High activity area
        else if (n > 2) env_activity = 1; // Medium
        // Restart async scan periodically
        static uint32_t lastScan = 0;
        if (millis() - lastScan > 60000) {
            WiFi.scanNetworks(true, true);  // Async, include hidden
            lastScan = millis();
        }
    }

    // Combine into state index (32 possible states)
    int state = bat_level + (time_state * 4) + (env_activity * 8);
    if (state >= STATES) state = STATES - 1;
    
    return state;
}

int q_choose_action(int state) {
    if (!ai_initialized) return 0;

    // Epsilon-greedy: dynamic exploration rate
    int explore_chance = (int)(epsilon * 100);
    if (random(100) < explore_chance) {
        int action = random(ACTIONS);
        Serial.printf("[AI] Exploring (ε=%.2f): random action %d\n", epsilon, action);
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

    // Track statistics
    total_updates++;
    total_rewards += reward;

    Serial.printf("[AI] Update: S%d A%d R%.2f -> Q%.3f (total: %d)\n",
                  state, action, reward, new_q, total_updates);

    // Save history
    save_history_entry(state, action, reward);
    
    // Periodic save to SD and decay exploration
    update_count++;
    if (update_count >= SAVE_INTERVAL) {
        q_save_table();
        ai_decay_exploration();
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

// ============================================================================
// NEW ENHANCED API IMPLEMENTATION
// ============================================================================

void ai_get_statistics(AIStats* stats) {
    if (!stats || !ai_initialized) return;
    
    stats->totalUpdates = total_updates;
    stats->totalRewards = (uint32_t)total_rewards;
    stats->avgReward = (total_updates > 0) ? (total_rewards / total_updates) : 0.0f;
    stats->explorationRate = epsilon;
    
    // Find best overall action
    float max_val = -INVALID_MAX;
    int best_action = 0;
    for (int s = 0; s < STATES; s++) {
        for (int a = 0; a < ACTIONS; a++) {
            float val = q_table[s * ACTIONS + a];
            if (val > max_val) {
                max_val = val;
                best_action = a;
            }
        }
    }
    stats->bestAction = best_action;
    stats->bestQValue = max_val;
    
    Serial.printf("[AI] Stats: updates=%d, avgR=%.2f, ε=%.2f, best=%d(Q=%.2f)\n",
                  stats->totalUpdates, stats->avgReward, stats->explorationRate,
                  stats->bestAction, stats->bestQValue);
}

bool ai_export_qtable_json(const char* path) {
    if (!ai_initialized || !q_table) return false;
    
    File file = SD.open(path, FILE_WRITE);
    if (!file) {
        Serial.printf("[AI] Failed to open: %s\n", path);
        return false;
    }
    
    file.println("{");
    file.printf("  \"version\": 2,\n");
    file.printf("  \"states\": %d,\n", STATES);
    file.printf("  \"actions\": %d,\n", ACTIONS);
    file.printf("  \"epsilon\": %.3f,\n", epsilon);
    file.printf("  \"total_updates\": %d,\n", total_updates);
    file.println("  \"action_names\": [");
    for (int a = 0; a < ACTIONS; a++) {
        file.printf("    \"%s\"%s\n", action_names[a], (a < ACTIONS-1) ? "," : "");
    }
    file.println("  ],");
    file.println("  \"q_table\": [");
    
    for (int s = 0; s < STATES; s++) {
        file.print("    [");
        for (int a = 0; a < ACTIONS; a++) {
            file.printf("%.4f", q_table[s * ACTIONS + a]);
            if (a < ACTIONS - 1) file.print(", ");
        }
        file.print("]");
        if (s < STATES - 1) file.println(",");
        else file.println();
    }
    
    file.println("  ]");
    file.println("}");
    file.close();
    
    Serial.printf("[AI] Q-table exported to: %s\n", path);
    return true;
}

void ai_set_exploration_rate(float new_epsilon) {
    epsilon = constrain(new_epsilon, 0.0f, 1.0f);
    Serial.printf("[AI] Exploration rate set to: %.2f\n", epsilon);
}

float ai_get_exploration_rate() {
    return epsilon;
}

void ai_decay_exploration() {
    // Decay from 30% to 5% over time
    const float min_epsilon = 0.05f;
    const float decay_rate = 0.995f;  // Slow decay
    
    if (epsilon > min_epsilon) {
        epsilon *= decay_rate;
        if (epsilon < min_epsilon) epsilon = min_epsilon;
        Serial.printf("[AI] Epsilon decayed to: %.3f\n", epsilon);
    }
}

void ai_reset_qtable() {
    if (!q_table) return;
    
    // Reset all values to optimistic 0.5
    for (int i = 0; i < STATES * ACTIONS; i++) {
        q_table[i] = 0.5f;
    }
    
    // Reset statistics
    total_updates = 0;
    total_rewards = 0.0f;
    epsilon = 0.30f;  // Reset exploration
    
    // Save empty Q-table
    q_save_table();
    
    Serial.println("[AI] Q-table reset to initial values");
}

