/**
 * @file wifite_auto.h
 * @brief Modo Wifite Automatizado - Scan, Select, Attack
 * @author Lele Origin Team
 * @version 1.0
 */

#ifndef __WIFITE_AUTO_H__
#define __WIFITE_AUTO_H__

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <vector>

/**
 * @brief Estrutura de rede WiFi com info de clientes
 */
struct WifiteNetwork {
    uint8_t bssid[6];
    char ssid[33];
    uint8_t channel;
    int8_t rssi;
    wifi_auth_mode_t authmode;
    int clientCount;        // Clientes detectados (probe requests)
    bool hasHandshake;      // Se já capturamos handshake
    bool pmkidCaptured;     // Se PMKID foi capturado
};

/**
 * @brief Modo de ataque Wifite
 */
enum WifiteMode {
    WIFITE_MANUAL = 0,      // Seleção manual
    WIFITE_AUTO_BEST,       // Automatico - melhor sinal
    WIFITE_AUTO_CLIENTS,    // Automatico - mais clientes
    WIFITE_AUTO_ALL         // Ataca todos sequencialmente
};

// Inicializa módulo Wifite
void wifite_init();

// Menu principal Wifite
void wifite_menu();

// Scan com contagem de clientes
int wifite_scan(bool withClientCount = true);

// Obtém lista de redes
#define MAX_WIFITE_NETWORKS 50

// Obtém rede por índice
WifiteNetwork* wifite_get_network(int index);

// Obtém contagem de redes
int wifite_get_network_count();

// Ataque automatizado
void wifite_auto_attack(WifiteMode mode);

// Ataca rede específica
bool wifite_attack_network(WifiteNetwork& net);

// Para ataques
void wifite_stop();

// Salva resultados no SD
bool wifite_save_results(const char* filepath);

#endif // __WIFITE_AUTO_H__
