#ifndef Q_LEARN_IA_H
#define Q_LEARN_IA_H

#include <Arduino.h>

void setup_q_learn();
void q_update(int state, int action, float reward);
int q_choose_action(int state);

#endif
