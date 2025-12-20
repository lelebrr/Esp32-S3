/**
 * @file nrf24_attacks.h
 * @brief NRF24L01+ Attack Definitions
 * 
 * Header file for NRF24 attack logic, including MouseJack
 * and Keyboard Sniffing class definitions.
 * 
 * @author Monster S3 Team
 * @date 2025
 */

#ifndef NRF24_ATTACKS_H
#define NRF24_ATTACKS_H

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

// ============================================================================
// NRF24 ATTACK TYPES
// ============================================================================
typedef enum {
    NRF_IDLE = 0,
    NRF_SCANNING,
    NRF_SNIFFING,
    NRF_INJECTING,
    NRF_JAMMING
} NRFState;

// ============================================================================
// NRF24 CORE CLASS
// ============================================================================
class NRF24Attacks {
public:
    /**
     * @brief Initialize NRF24L01+ module
     * @return true if successful
     */
    static bool init();

    /**
     * @brief Stop radio and power down
     */
    static void stop();

    /**
     * @brief Check if module is initialized
     */
    static bool isInitialized();
    
    // Mousejack Attacks
    // Mousejack Attacks
    /**
     * @brief Start scanning for vulnerable Microsoft/Logitech mice
     */
    static void startMousejackScan();

    /**
     * @brief Update scan/injection logic loop
     */
    static void updateMousejack();

    /**
     * @brief Start injecting payloads into a target address
     * @param address Target MAC address (5 bytes)
     * @param channel Target channel
     */
    static void startInjection(uint8_t* address, uint8_t channel);
    
    // Keyboard Sniffing (Microsoft / Logitech)
    /**
     * @brief Start sniffing for Unencrypted HID packets
     */
    static void startKeyboardSniff();

    /**
     * @brief Update sniffer logic loop
     */
    static void updateKeyboardSniff();
    
    // Jammer
    /**
     * @brief Start 2.4GHz Constant Carrier Jammer
     * @param channel Channel to jam (0-125)
     */
    static void startJammer24(uint8_t channel);

    /**
     * @brief Update jammer loop
     */
    static void updateJammer24();

    /**
     * @brief Stop jammer
     */
    static void stopJammer24();
    
    // Status
    static NRFState getState();
    static int8_t getRSSI();
    
private:
    static RF24 _radio;
    static bool _initialized;
    static NRFState _state;
    static uint8_t _channel;
    
    // Mousejack state
    static uint32_t _lastScanTime;
    static uint8_t _scanChannel;
    static uint8_t _targetAddress[5];
    
    // Helpers
    static void setChannel(uint8_t ch);
    static void scanChannels();
};

#endif // NRF24_ATTACKS_H
