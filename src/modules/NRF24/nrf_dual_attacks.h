/**
 * @file nrf_dual_attacks.h
 * @brief Header for Dual NRF24 Attack Modules
 * @author Lele Origin Team
 */

#ifndef __NRF_DUAL_ATTACKS_H__
#define __NRF_DUAL_ATTACKS_H__

void nrf_dual_jammer();        // Carrier Wave on 2 channels
void nrf_hyper_spam();         // BLE Adv Spam on 2 channels parallel
void nrf_protocol_hunter();    // WiFi Jammer + BLE Jammer
void nrf_mouse_jack_dual();    // Sniff & Inject parallel

#endif // __NRF_DUAL_ATTACKS_H__
