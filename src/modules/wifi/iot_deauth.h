/**
 * @file iot_deauth.h
 * @brief Módulo de Deauth Storm para dispositivos IoT
 * @author Lele Origin Team
 * @version 1.0
 * @date 2024-12-13
 *
 * Este módulo implementa ataques de desautenticação direcionados
 * a dispositivos IoT específicos, identificados por OUI (fabricante):
 * - Chromecast (Google Nest, Chromecast)
 * - Alexa (Amazon Echo, Fire TV)
 * - Smart TVs (Samsung, LG)
 *
 * @warning Use apenas para fins educacionais e em redes autorizadas.
 */

#ifndef __IOT_DEAUTH_H__
#define __IOT_DEAUTH_H__

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

/**
 * @brief OUIs conhecidos de dispositivos Chromecast (Google)
 */
static const uint8_t OUI_CHROMECAST[][3] = {
    {0xF4, 0xF5, 0xD8},  ///< Google Nest
    {0x54, 0x60, 0x09},  ///< Google LLC
    {0x94, 0xEB, 0x2C},  ///< Google
    {0xD4, 0xF5, 0x47},  ///< Google
    {0xE8, 0xDE, 0x27},  ///< TP-Link (algumas marcas)
};

/**
 * @brief OUIs conhecidos de dispositivos Amazon Alexa/Echo
 */
static const uint8_t OUI_ALEXA[][3] = {
    {0xA0, 0x02, 0xDC},  ///< Amazon
    {0x74, 0xC2, 0x46},  ///< Amazon
    {0x40, 0xB4, 0xCD},  ///< Amazon Technologies
    {0xF0, 0x81, 0x75},  ///< Amazon
    {0x44, 0x65, 0x0D},  ///< Amazon
};

/**
 * @brief OUIs conhecidos de Smart TVs (Samsung, LG)
 */
static const uint8_t OUI_SMARTTV[][3] = {
    {0x8C, 0x79, 0xF5},  ///< Samsung
    {0xE4, 0x7C, 0xF9},  ///< Samsung
    {0x00, 0xE0, 0x91},  ///< LG
    {0x3C, 0xCD, 0x5D},  ///< LG
    {0xA8, 0x16, 0xB2},  ///< LG
    {0x10, 0x68, 0x3F},  ///< LG
};

/**
 * @brief Tipo de alvo IoT
 */
enum IoTTarget {
    IOT_TARGET_CHROMECAST = 0,  ///< Dispositivos Google
    IOT_TARGET_ALEXA,           ///< Dispositivos Amazon
    IOT_TARGET_SMARTTV,         ///< Smart TVs
    IOT_TARGET_ALL              ///< Todos os IoT
};

/**
 * @brief Estrutura de dispositivo IoT encontrado
 */
struct IoTDevice {
    uint8_t bssid[6];       ///< MAC do AP
    uint8_t client_mac[6];  ///< MAC do cliente IoT
    uint8_t channel;        ///< Canal WiFi
    int8_t rssi;            ///< Força do sinal
    IoTTarget type;         ///< Tipo de dispositivo
    bool is_active;         ///< Se está ativo no ataque
    uint32_t packets_sent;  ///< Pacotes enviados para este alvo
};

/**
 * @brief Inicializa o módulo de IoT Deauth
 */
void iot_deauth_init();

/**
 * @brief Escaneia dispositivos IoT do tipo especificado
 * @param type Tipo de alvo (IOT_TARGET_CHROMECAST, etc.)
 * @return Número de dispositivos encontrados
 */
int iot_deauth_scan(IoTTarget type);

/**
 * @brief Inicia o ataque de desautenticação
 * @return true se iniciou com sucesso
 * @pre Deve chamar iot_deauth_scan() antes
 */
bool iot_deauth_start();

/**
 * @brief Para o ataque de desautenticação
 */
void iot_deauth_stop();

/**
 * @brief Atualiza o ataque (deve ser chamado no loop)
 * @return Número de pacotes enviados nesta iteração
 */
int iot_deauth_update();

/**
 * @brief Verifica se o ataque está ativo
 * @return true se está atacando
 */
bool iot_deauth_is_active();

/**
 * @brief Obtém total de pacotes enviados
 * @return Contador de pacotes
 */
uint32_t iot_deauth_get_packets();

/**
 * @brief Obtém número de dispositivos alvo
 * @return Quantidade de alvos
 */
int iot_deauth_get_device_count();

/**
 * @brief Obtém lista de dispositivos
 * @return Ponteiro para array de IoTDevice
 */
IoTDevice* iot_deauth_get_devices();

#endif // __IOT_DEAUTH_H__
