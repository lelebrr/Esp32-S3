/**
 * @file wps_attacks.h
 * @brief Ataques WPS: Pixie Dust e Brute Force
 * 
 * Implementa ataques contra WPS vulnerável.
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#ifndef WPS_ATTACKS_H
#define WPS_ATTACKS_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Status do ataque WPS
typedef enum {
    WPS_STATUS_IDLE,
    WPS_STATUS_SCANNING,
    WPS_STATUS_PIXIE_ATTEMPT,
    WPS_STATUS_BRUTE_ATTEMPT,
    WPS_STATUS_SUCCESS,
    WPS_STATUS_FAILED,
    WPS_STATUS_LOCKED
} WPSAttackStatus;

// Rede WPS
typedef struct {
    char ssid[33];
    uint8_t bssid[6];
    int8_t rssi;
    uint8_t channel;
    bool wpsEnabled;
    bool wpsLocked;
} WPSNetwork;

class WPSAttacks {
public:
    /**
     * @brief Escaneia redes com WPS habilitado
     * @return Número de redes WPS encontradas
     */
    static int scanWPS();
    
    /**
     * @brief Retorna rede WPS por índice
     */
    static WPSNetwork getNetwork(int index);
    
    /**
     * @brief Inicia ataque Pixie Dust
     * @param target Rede alvo
     * @return true se ataque iniciado
     */
    static bool startPixieDust(const WPSNetwork& target);
    
    /**
     * @brief Inicia ataque brute force WPS PIN
     * @param target Rede alvo
     * @return true se ataque iniciado
     */
    static bool startBruteForce(const WPSNetwork& target);
    
    /**
     * @brief Atualiza ataque em andamento
     * @return Status atual
     */
    static WPSAttackStatus update();
    
    /**
     * @brief Para ataque atual
     */
    static void stop();
    
    /**
     * @brief Retorna status atual
     */
    static WPSAttackStatus getStatus();
    
    /**
     * @brief Retorna PIN descoberto (se sucesso)
     */
    static String getDiscoveredPIN();
    
    /**
     * @brief Retorna senha descoberta (se sucesso)
     */
    static String getDiscoveredPassword();
    
    /**
     * @brief Retorna progresso do brute (0-11000)
     */
    static uint16_t getBruteProgress();

private:
    static WPSNetwork _networks[20];
    static int _networkCount;
    static WPSAttackStatus _status;
    static WPSNetwork _currentTarget;
    
    // Pixie Dust
    static uint8_t _enrolleeNonce[16];
    static uint8_t _registrarNonce[16];
    static uint8_t _enrolleePubKey[192];
    static uint8_t _eHash1[32];
    static uint8_t _eHash2[32];
    
    // Brute Force
    static uint16_t _currentPIN;
    static String _discoveredPIN;
    static String _discoveredPassword;
    
    // Helpers
    static uint8_t calculateChecksum(uint32_t pin);
    static bool sendEAPOLStart(const uint8_t* bssid);
    static bool processM1(const uint8_t* data, size_t len);
    static bool processM3(const uint8_t* data, size_t len);
    static bool tryPixieCalculation();
    static bool tryPIN(uint16_t pin);
};

#endif // WPS_ATTACKS_H
