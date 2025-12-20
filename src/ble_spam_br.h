/**
 * @file ble_spam_br.h
 * Monster S3 Firmware - BLE Spam Header
 */

#ifndef BLE_SPAM_BR_H
#define BLE_SPAM_BR_H

#include <stdint.h>

// Spam modes
typedef enum {
    BLE_SPAM_ALL = 0,    // All device types
    BLE_SPAM_APPLE,      // Apple devices only
    BLE_SPAM_SAMSUNG,    // Samsung devices only
    BLE_SPAM_XIAOMI      // Xiaomi devices only
} BLESpamMode;

// Initialize BLE spam subsystem
void ble_spam_init();

// Start spam attack
void ble_spam_start(BLESpamMode mode);

// Update (call in loop)
uint32_t ble_spam_update();

// Stop attack
void ble_spam_stop();

// Check if running
bool ble_spam_is_running();

// Get packet count
uint32_t ble_spam_get_packet_count();

#endif // BLE_SPAM_BR_H
