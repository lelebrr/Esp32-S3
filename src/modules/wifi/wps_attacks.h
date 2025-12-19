/**
 * @file wps_attacks.h
 * @brief Módulo de ataques WPS para o Lele Origin
 * @author Lele Origin Team
 * @version 1.0
 * @date 2024-12-13
 *
 * Este módulo implementa ataques contra o protocolo WPS (WiFi Protected Setup):
 * - Scan de redes WPS habilitadas
 * - Pixie Dust (ataque offline via vulnerabilidade E-Hash)
 * - Bruteforce de PIN (11000 combinações)
 * - PIN Flood DoS (envia M4 malformados para travar WPS)
 *
 * @warning Use apenas para fins educacionais e em redes autorizadas.
 */

#ifndef __WPS_ATTACKS_H__
#define __WPS_ATTACKS_H__

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <vector>

/**
 * @brief Status do ataque WPS
 */
enum WPSStatus {
    WPS_IDLE = 0,      ///< Ocioso, nenhum ataque em andamento
    WPS_SCANNING,      ///< Escaneando redes WPS
    WPS_ATTACKING,     ///< Ataque em andamento
    WPS_SUCCESS,       ///< Ataque bem-sucedido
    WPS_FAILED,        ///< Ataque falhou
    WPS_LOCKED         ///< WPS travado pelo roteador
};

/**
 * @brief Estrutura de rede WPS encontrada
 */
struct WPSNetwork {
    uint8_t bssid[6];    ///< MAC do AP
    char ssid[33];       ///< Nome da rede
    uint8_t channel;     ///< Canal WiFi
    int8_t rssi;         ///< Força do sinal
    bool wps_enabled;    ///< WPS habilitado
    bool wps_locked;     ///< WPS travado
};

/**
 * @brief Inicializa o módulo de ataques WPS
 */
void wps_init();

/**
 * @brief Escaneia redes com WPS habilitado
 * @return Número de redes WPS encontradas
 */
int wps_scan_networks();

/**
 * @brief Obtém lista de redes WPS escaneadas
 * @return Vetor de redes WPS
 */
std::vector<WPSNetwork>& wps_get_networks();

/**
 * @brief Inicia ataque Pixie Dust
 * @param bssid MAC do AP alvo
 * @param channel Canal WiFi
 * @return true se iniciou com sucesso
 * @note Requer análise offline do E-Hash
 */
bool wps_start_pixie_dust(uint8_t* bssid, uint8_t channel);

/**
 * @brief Inicia bruteforce de PIN WPS
 * @param bssid MAC do AP alvo
 * @param channel Canal WiFi
 * @param start_pin PIN inicial (0-99999999)
 * @return true se iniciou com sucesso
 */
bool wps_start_bruteforce(uint8_t* bssid, uint8_t channel, uint32_t start_pin);

/**
 * @brief Inicia PIN Flood DoS
 * @param bssid MAC do AP alvo
 * @param channel Canal WiFi
 * @return true se iniciou com sucesso
 * @note Envia pacotes M4 malformados para travar WPS
 */
bool wps_start_pin_flood(uint8_t* bssid, uint8_t channel);

/**
 * @brief Para qualquer ataque em andamento
 */
void wps_stop_attack();

/**
 * @brief Atualiza estado do ataque (deve ser chamado no loop)
 */
void wps_update();

/**
 * @brief Obtém status atual do ataque
 * @return WPSStatus atual
 */
WPSStatus wps_get_status();

/**
 * @brief Obtém número de PINs tentados
 * @return Quantidade de PINs testados
 */
uint32_t wps_get_pins_tried();

/**
 * @brief Obtém PIN descoberto (se sucesso)
 * @return String do PIN no formato 8 dígitos
 */
const char* wps_get_discovered_pin();

#endif // __WPS_ATTACKS_H__
