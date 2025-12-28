#ifndef Q_LEARN_IA_H
#define Q_LEARN_IA_H

#include <Arduino.h>

// ============================================================================
// CONFIGURATION CONSTANTS
// ============================================================================

#define AI_STATES   32   // 4 battery * 2 time * 4 activity
#define AI_ACTIONS  8    // None, BLE, WiFi, NFC, SubGHz, IR, WPS, KRACK

// ============================================================================
// AI STATISTICS STRUCTURE
// ============================================================================

struct AIStats {
    uint32_t totalUpdates;
    uint32_t totalRewards;
    float avgReward;
    float explorationRate;
    int bestAction;
    float bestQValue;
};

// ============================================================================
// PUBLIC API
// ============================================================================

/**
 * @brief Initialize Q-Learning system
 * Allocates Q-table in PSRAM and loads from SD if available
 */
void setup_q_learn();

/**
 * @brief Update Q-value for state-action pair
 * @param state Environment state (0-31)
 * @param action Action taken (0-7)
 * @param reward Reward received (-1.0 to 1.0)
 */
void q_update(int state, int action, float reward);

/**
 * @brief Choose action using epsilon-greedy policy
 * @param state Current environment state
 * @return Action index (0-7)
 */
int q_choose_action(int state);

/**
 * @brief Get current environment state
 * Considers: battery, time of day, WiFi activity, GPS location
 * @return State index (0-31)
 */
int get_env_state();

/**
 * @brief Main AI loop - call from main loop
 * Takes decisions every 30s when in combat mode
 */
void ai_loop_step();

/**
 * @brief Enable/disable autonomous combat mode
 * @param enabled true to allow AI to execute attacks
 */
void ai_set_combat_mode(bool enabled);

/**
 * @brief Check if combat mode is active
 * @return true if AI can execute attacks
 */
bool ai_get_combat_mode();

/**
 * @brief Save Q-table to SD card
 * @return true on success
 */
bool q_save_table();

/**
 * @brief Load Q-table from SD card
 * @return true on success
 */
bool q_load_table();

/**
 * @brief Give reward to AI for current action
 * @param reward Reward value (typically +1 or -1)
 */
void ai_give_reward(float reward);

/**
 * @brief Report successful attack (+1 reward)
 */
void ai_report_success();

/**
 * @brief Report failed attack (-1 reward)
 */
void ai_report_failure();

// ============================================================================
// NEW ENHANCED API
// ============================================================================

/**
 * @brief Get AI training statistics
 * @param stats Output structure with statistics
 */
void ai_get_statistics(AIStats* stats);

/**
 * @brief Export Q-table as JSON to SD card
 * @param path Output file path (e.g., "/ai/qtable.json")
 * @return true on success
 */
bool ai_export_qtable_json(const char* path);

/**
 * @brief Set exploration rate (epsilon)
 * @param epsilon Value from 0.0 (exploit only) to 1.0 (explore only)
 */
void ai_set_exploration_rate(float epsilon);

/**
 * @brief Get current exploration rate
 * @return Current epsilon value
 */
float ai_get_exploration_rate();

/**
 * @brief Decay exploration rate over time
 * Called automatically, reduces epsilon gradually
 */
void ai_decay_exploration();

/**
 * @brief Reset Q-table to initial values
 * Clears all learned values
 */
void ai_reset_qtable();

#endif // Q_LEARN_IA_H

