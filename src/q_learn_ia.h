/**
 * @file q_learn_ia.h
 * Monster S3 Firmware - Q-Learning AI Header
 */

#ifndef Q_LEARN_IA_H
#define Q_LEARN_IA_H

#include <stdint.h>

// Initialize Q-learning system
void setup_q_learn();

// Choose best action for given state
int q_choose_action(int state);

// Update Q-value after action
void q_update(int state, int action, float reward);

// User feedback buttons
void ai_feedback_success();  // "Dá certo" button
void ai_feedback_fail();     // "Dá ruim" button

// Force save Q-table to SD
void ai_save();

// Main loop step (call every frame)
void ai_loop_step();

// Get AI statistics
void ai_get_stats(uint32_t* decisions, float* epsilon);

#endif // Q_LEARN_IA_H
