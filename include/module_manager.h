/**
 * @file module_manager.h
 * @brief Gerenciador de Módulos Exclusivos com Mutex
 *
 * Implementa exclusão mútua entre módulos para evitar conflitos:
 * - BLE ativo = WiFi desligado
 * - WiFi ativo = BLE desligado
 * - SubGHz ativo = IR desligado
 * - Fault ativo = Tudo desligado
 *
 * @author MorphNode Team
 * @date 2025-12-23
 */

#ifndef MODULE_MANAGER_H
#define MODULE_MANAGER_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Tipos de módulo
typedef enum {
    MODULE_NONE = 0,
    MODULE_BLE,
    MODULE_WIFI,
    MODULE_SUBGHZ,
    MODULE_IR,
    MODULE_NFC,
    MODULE_FAULT,
    MODULE_GPS,
    MODULE_USB
} ModuleType;

class ModuleManager {
public:
    /**
     * @brief Inicializa o gerenciador com mutex
     */
    static void init();

    /**
     * @brief Desliga TODOS os módulos
     */
    static void desligaModulos();

    /**
     * @brief Ativa um módulo específico (desliga conflitantes)
     * @param module Módulo a ativar
     * @return true se conseguiu ativar
     */
    static bool ativaModulo(ModuleType module);

    /**
     * @brief Desativa módulo atual
     */
    static void desativaModulo();

    /**
     * @brief Obtém mutex para operação crítica
     * @return true se obteve o mutex
     */
    static bool takeMutex(uint32_t timeout_ms = portMAX_DELAY);

    /**
     * @brief Libera mutex após operação
     */
    static void giveMutex();

    /**
     * @brief Retorna módulo atualmente ativo
     */
    static ModuleType getActiveModule();

    /**
     * @brief Verifica se módulo está ativo
     */
    static bool isModuleActive(ModuleType module);

    /**
     * @brief Detecta hardware conectado no boot
     */
    static void detectHardware();

    /**
     * @brief Verifica se o hardware do módulo está presente
     */
    static bool isHardwareConnected(ModuleType module);

private:
    static SemaphoreHandle_t _mutex;
    static ModuleType _activeModule;

    static void desligaBLE();
    static void desligaWiFi();
    static void desligaSubGHz();
    static void desligaIR();
    static void desligaNFC();
    static void desligaFault();
};

#endif // MODULE_MANAGER_H
