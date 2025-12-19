#ifndef Q_LEARN_IA_H
#define Q_LEARN_IA_H

#include <stdint.h>
#include <stdbool.h>

// Funções públicas para controle da IA Q-learning otimizada
bool start_q_learning();
void stop_q_learning();
bool init_q_table();
bool save_q_table_to_sd();
bool load_q_table_from_sd();
uint32_t get_learning_iterations();
uint32_t get_successful_actions();
float get_average_reward();
bool is_q_learning_active();
void cleanup_q_table();

#endif // Q_LEARN_IA_H
