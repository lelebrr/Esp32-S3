#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

/**
 * @brief Initialize logging system
 * Creates /logs directory if needed
 */
void log_init();

/**
 * @brief Log an attack event
 * @param attack_name Name of the attack
 * @param success true if attack succeeded
 */
void log_attack(const char* attack_name, bool success);

/**
 * @brief Log battery status
 * @param level Battery percentage (0-100)
 * @param voltage Battery voltage
 */
void log_battery(int level, float voltage);

/**
 * @brief Log system event
 * @param event Event description
 */
void log_system(const char* event);

/**
 * @brief Log AI decision
 * @param state Environment state
 * @param action Action chosen
 * @param reward Reward received
 */
void log_ai(int state, int action, float reward);

/**
 * @brief Flush all log buffers to SD
 */
void log_flush();

#endif // LOGGER_H
