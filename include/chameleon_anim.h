#ifndef CHAMELEON_ANIM_H
#define CHAMELEON_ANIM_H

#include <lvgl.h>

enum ChameleonMood {
    MOOD_IDLE,
    MOOD_ATTACK,
    MOOD_SUCCESS,
    MOOD_ERROR,
    MOOD_SLEEP,
    MOOD_EATING,    // Tongue extension
    MOOD_SNEAKY,    // Camouflage/Stealth
    MOOD_EXCITED,   // RGB/Party
    MOOD_CONFUSED   // Crazy eyes spin
};

struct ChameleonConfig {
    uint32_t color_idle;
    uint32_t color_attack;
    uint32_t color_success;
    uint32_t color_error;
    uint32_t color_sneaky;
    uint32_t color_eating;
    uint32_t color_party; // Base color if not rainbow
    bool rainbow_party;   // Toggle rainbow effect
    int anim_speed;       // 1-10
};

class ChameleonAnim {
public:
    static void init(lv_obj_t* parent, int x, int y);
    static void stop(); // Cleanup timer and pointers
    static void setMood(ChameleonMood mood);
    static void loop(); // Call in LVGL loop if needed, or use timer
    
    // Config
    static void loadConfig();
    static void saveConfig();
    static void setConfig(const ChameleonConfig& cfg);
    static ChameleonConfig getConfig();

private:
    static ChameleonConfig config;
    static lv_obj_t* container;
    static lv_obj_t* head;
    static lv_obj_t* body;
    static lv_obj_t* tail;
    static lv_obj_t* tongue; // New tongue object
    static lv_obj_t* eye_left;
    static lv_obj_t* eye_right;
    static lv_obj_t* pupil_left;
    static lv_obj_t* pupil_right;
    
    static ChameleonMood currentMood;
    static lv_timer_t* anim_timer;
    
    static void anim_cb(lv_timer_t* timer);
    static void update_colors();
    static void update_eyes();
};

#endif // CHAMELEON_ANIM_H
