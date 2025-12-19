/**
 * @file attack_commands.h
 * @brief Comandos CLI para Ataques WiFi, BLE, IoT e Console
 * @author Lele Origin Team
 * @version 1.0
 */

#ifndef ATTACK_COMMANDS_H
#define ATTACK_COMMANDS_H

#include <SimpleCLI.h>

/**
 * @brief Registra todos os comandos de ataque no CLI
 * @param cli Ponteiro para instância SimpleCLI
 */
void createAttackCommands(SimpleCLI *cli);

#endif // ATTACK_COMMANDS_H
