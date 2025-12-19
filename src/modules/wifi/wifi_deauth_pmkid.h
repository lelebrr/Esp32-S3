#ifndef WIFI_DEAUTH_PMKID_H
#define WIFI_DEAUTH_PMKID_H

#include <stdint.h>
#include <stdbool.h>
#include <esp_wifi_types.h>

// Funções públicas para controle do WiFi deauth otimizado com PMKID capture
bool start_wifi_deauth_pmkid();
void stop_wifi_deauth_pmkid();
bool set_deauth_target(const wifi_ap_record_t* ap_record);
uint32_t get_deauth_packets_sent();
uint32_t get_pmkid_captures();
bool is_wifi_deauth_active();
bool save_pmkid_to_sd();

#endif // WIFI_DEAUTH_PMKID_H
