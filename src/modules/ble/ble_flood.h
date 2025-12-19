/**
 * @file ble_flood.h
 * @brief Módulo de BLE Flood para spam de dispositivos
 * @author Lele Origin Team
 * @version 1.0
 * @date 2024-12-13
 *
 * Este módulo implementa ataques de BLE Flood (spam):
 * - Apple (AirPods, Apple Watch - pop-ups de pareamento)
 * - Android (Fast Pair - notificações de dispositivos)
 * - Samsung (Galaxy Buds, SmartTag)
 * - Windows (Swift Pair - pop-ups de pareamento)
 *
 * @warning Use apenas para fins educacionais e em ambientes autorizados.
 */

#ifndef __BLE_FLOOD_H__
#define __BLE_FLOOD_H__

#include <Arduino.h>

/**
 * @brief Modos de BLE Flood
 */
enum BLEFloodMode {
    BLE_FLOOD_APPLE = 0,   ///< Spam de dispositivos Apple (AirPods)
    BLE_FLOOD_ANDROID,     ///< Spam de Fast Pair (Google)
    BLE_FLOOD_SAMSUNG,     ///< Spam de Galaxy Buds, Watch
    BLE_FLOOD_WINDOWS,     ///< Spam de Swift Pair
    BLE_FLOOD_ALL          ///< Todos os modos simultaneamente
};

/**
 * @brief Inicializa o módulo de BLE Flood
 * @note Inicializa NimBLE se necessário
 */
void ble_flood_init();

/**
 * @brief Inicia o ataque de BLE Flood
 * @param mode Modo de ataque (BLE_FLOOD_APPLE, etc.)
 * @return true se iniciou com sucesso
 */
bool ble_flood_start(BLEFloodMode mode);

/**
 * @brief Para o ataque de BLE Flood
 */
void ble_flood_stop();

/**
 * @brief Atualiza o ataque (deve ser chamado no loop)
 * @return Número de pacotes enviados nesta iteração
 */
int ble_flood_update();

/**
 * @brief Verifica se o ataque está ativo
 * @return true se está atacando
 */
bool ble_flood_is_active();

/**
 * @brief Obtém total de pacotes enviados
 * @return Contador de pacotes
 */
uint32_t ble_flood_get_packets();

#endif // __BLE_FLOOD_H__
