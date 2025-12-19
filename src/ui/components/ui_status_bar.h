#pragma once

/**
 * @file ui_status_bar.h
 * @brief Header e Footer fixos para a UI
 * 
 * Header (topo): Bateria, WiFi, Hora, Status atual
 * Footer (baixo): Navegação PREV/SEL/NEXT ou info contextual
 * 
 * Tela: 368x448px
 * Header: 40px altura
 * Footer: 35px altura
 * Área útil: 373px (448 - 40 - 35)
 */

#include <Arduino.h>
#include <lvgl.h>

#define STATUS_BAR_HEIGHT 40
#define NAV_BAR_HEIGHT 35
#define USABLE_HEIGHT (LCD_HEIGHT - STATUS_BAR_HEIGHT - NAV_BAR_HEIGHT)

/**
 * @brief Status Bar (Header) - Topo da tela
 */
class UIStatusBar {
public:
    UIStatusBar();
    
    /** Cria o header na tela */
    void create(lv_obj_t* parent);
    
    /** Atualiza valores */
    void setBattery(int percent, bool charging);
    void setWiFi(bool connected, int strength);
    void setBLE(bool active);
    void setTime(int hour, int minute);
    void setStatus(const char* text);
    
    /** Update periódico (chamar a cada segundo) */
    void update();
    
    /** Mostra/Oculta */
    void show();
    void hide();
    
    /** Retorna a altura para calcular área útil */
    int getHeight() const { return STATUS_BAR_HEIGHT; }

private:
    lv_obj_t* _container;
    lv_obj_t* _batteryLabel;
    lv_obj_t* _wifiLabel;
    lv_obj_t* _bleLabel;
    lv_obj_t* _timeLabel;
    lv_obj_t* _statusLabel;
    
    int _batteryPercent;
    bool _charging;
    bool _wifiConnected;
    bool _bleActive;
};

/**
 * @brief Navigation Bar (Footer) - Base da tela
 */
class UINavBar {
public:
    UINavBar();
    
    /** Cria o footer na tela */
    void create(lv_obj_t* parent);
    
    /** Define callbacks de navegação */
    void setOnPrev(void (*callback)());
    void setOnSelect(void (*callback)());
    void setOnNext(void (*callback)());
    
    /** Atualiza labels dos botões (para contexto) */
    void setLabels(const char* prev, const char* select, const char* next);
    
    /** Mostra/Oculta */
    void show();
    void hide();
    
    /** Retorna a altura */
    int getHeight() const { return NAV_BAR_HEIGHT; }

private:
    lv_obj_t* _container;
    lv_obj_t* _prevBtn;
    lv_obj_t* _selectBtn;
    lv_obj_t* _nextBtn;
    lv_obj_t* _prevLabel;
    lv_obj_t* _selectLabel;
    lv_obj_t* _nextLabel;
    
    void (*_onPrev)();
    void (*_onSelect)();
    void (*_onNext)();
    
    static void onPrevClick(lv_event_t* e);
    static void onSelectClick(lv_event_t* e);
    static void onNextClick(lv_event_t* e);
};

// Instâncias globais (renomeadas para evitar conflito)
extern UIStatusBar uiStatusBar;
extern UINavBar uiNavBar;
