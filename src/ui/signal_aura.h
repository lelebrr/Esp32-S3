#pragma once
/**
 * @file signal_aura.h
 * @brief Aura visual de força de sinal (WiFi/BLE)
 */

#include <Arduino.h>
#include <lvgl.h>

class SignalAura {
public:
    SignalAura();
    
    void init(lv_obj_t* parent);
    void update();
    
    void setSignalStrength(uint8_t strength);
    void setRSSI(int8_t rssi);
    
    void show();
    void hide();
    
    void setColor(uint32_t color);
    void setPulsing(bool enable);
    
private:
    void updateRings();
    uint32_t getColorForStrength(uint8_t strength);
    
    lv_obj_t* _container;
    lv_obj_t* _auraRings[3];
    uint8_t _strength;
    uint32_t _color;
    bool _visible;
    bool _pulsing;
    uint32_t _frameCount;
};

extern SignalAura signalAura;
