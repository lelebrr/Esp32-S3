/**
 * @file ble_attacks.h
 * @brief Ataques BLE Otimizados com 1200+ pps
 *
 * Spam BLE de alta taxa com nomes brasileiros.
 *
 * @author MorphNode Team
 * @date 2025-12-23
 */

#ifndef BLE_ATTACKS_H
#define BLE_ATTACKS_H

#include <Arduino.h>
#include <NimBLEDevice.h>

// Tipos de ataque BLE
typedef enum {
    BLE_ATTACK_NONE,
    BLE_ATTACK_SPAM_GENERIC,
    BLE_ATTACK_SPAM_BRAZIL,    // Nomes Vivo, Oi, Claro, Tim
    BLE_ATTACK_SOUR_APPLE,     // iOS popup flood
    BLE_ATTACK_SWIFT_PAIR,     // Windows popup
    BLE_ATTACK_FAST_PAIR,      // Android popup
    BLE_ATTACK_SAMSUNG_BUDS,   // Samsung spam
    BLE_ATTACK_SKIMMER_DETECT, // Detecta skimmers BLE
    BLE_ATTACK_DEAUTH          // Disconnection attack
} BLEAttackType;

class BLEAttacks {
public:
    /**
     * @brief Inicializa sistema BLE
     */
    static void init();

    /**
     * @brief Inicia ataque específico
     * @param type Tipo de ataque
     */
    static void start(BLEAttackType type);

    /**
     * @brief Para ataque atual
     */
    static void stop();

    /**
     * @brief Atualiza ataque (chamar no loop, alta frequência)
     */
    static void update();

    /**
     * @brief Verifica se ataque está ativo
     */
    static bool isActive();

    /**
     * @brief Retorna contagem de pacotes enviados
     */
    static uint32_t getPacketCount();

    /**
     * @brief Retorna PPS atual (packets per second)
     */
    static uint16_t getCurrentPPS();

    /**
     * @brief Configura intervalo de adversiting (menor = mais rápido)
     * @param minMs Mínimo em ms (padrão: 20)
     * @param maxMs Máximo em ms (padrão: 40)
     */
    static void setAdvertisingInterval(uint16_t minMs, uint16_t maxMs);

private:
    static BLEAttackType _currentAttack;
    static bool _active;
    static NimBLEAdvertising *_advertising;

    static uint32_t _packetCount;
    static uint32_t _lastPPSTime;
    static uint32_t _ppsCounter;
    static uint16_t _currentPPS;

    static uint8_t _nameIndex;
    static uint8_t _deviceIndex;

    // Attack handlers
    static void updateSpamGeneric();
    static void updateSpamBrazil();
    static void updateSourApple();
    static void updateSwiftPair();
    static void updateFastPair();
    static void updateSamsungBuds();
    static void updateSkimmerDetect();

    // Helpers
    static void sendAdvertisement(const uint8_t *data, size_t len, const char *name = nullptr);
    static void randomizeMac();
};

#endif // BLE_ATTACKS_H
