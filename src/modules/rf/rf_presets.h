/**
 * @file rf_presets.h
 * @brief Header for RF Presets Module (Sub-GHz Attacks)
 * @author Lele Origin Team
 */

#ifndef __RF_PRESETS_H__
#define __RF_PRESETS_H__

#include <Arduino.h>

// Enum para tipos de presets
enum RFPresetType {
    RF_PRESET_TESLA_CHARGE = 0,
    RF_PRESET_CAME_12BIT,
    RF_PRESET_CAME_24BIT,
    RF_PRESET_NICE_FLOR,
    RF_PRESET_FAN_CONTROL,
    RF_PRESET_SUBGHZ_SPAM,
    RF_PRESET_JAMMER_SIMPLE
};

// Funções Principais
void rf_presets_init();
void rf_run_preset(RFPresetType type);
void rf_stop_preset();

// Funções Específicas (Expostas para uso granular se necessário)
void rf_play_tesla_charge();
void rf_play_came_12bit();
void rf_play_came_24bit();
void rf_play_fan_control();
void rf_run_rolling_code_sim();

#endif // __RF_PRESETS_H__
