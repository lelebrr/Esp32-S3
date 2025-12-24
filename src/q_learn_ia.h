#ifndef Q_LEARN_IA_H
#define Q_LEARN_IA_H

#include <Arduino.h>

/**
 * @brief Initialize Q-Learning system
 * Allocates Q-table in PSRAM and loads from SD if available
 */
void setup_q_learn();

/**
 * @brief Update Q-value for state-action pair
 * @param state Environment state (0-23)
 * @param action Action taken (0-5)
 * @param reward Reward received (-1.0 to 1.0)
 */
void q_update(int state, int action, float reward);

/**
 * @brief Choose action using epsilon-greedy policy
 * @param state Current environment state
 * @return Action index (0-5)
 */
int q_choose_action(int state);

/**
 * @brief Get current environment state
 * @return State index (0-23)
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

#endif // Q_LEARN_IA_H
