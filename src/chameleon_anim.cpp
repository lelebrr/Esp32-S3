#include "chameleon_anim.h"
#include <Arduino.h>
#include <vector>
#include "sound_manager.h"

// Static members
lv_obj_t* ChameleonAnim::container = nullptr;
lv_obj_t* ChameleonAnim::head = nullptr;
lv_obj_t* ChameleonAnim::body = nullptr;
lv_obj_t* ChameleonAnim::tail = nullptr;
lv_obj_t* ChameleonAnim::eye_left = nullptr;
lv_obj_t* ChameleonAnim::eye_right = nullptr;
lv_obj_t* ChameleonAnim::pupil_left = nullptr;
lv_obj_t* ChameleonAnim::pupil_right = nullptr;
lv_obj_t* ChameleonAnim::tongue = nullptr;

ChameleonMood ChameleonAnim::currentMood = MOOD_IDLE;
lv_timer_t* ChameleonAnim::anim_timer = nullptr;

ChameleonConfig ChameleonAnim::config = {
    .color_idle = 0x4ecdc4,
    .color_attack = 0xe94560,
    .color_success = 0x00ff00,
    .color_error = 0xff0000,
    .color_sneaky = 0x222222,
    .color_eating = 0xffa500, // Orange default
    .color_party = 0xff00ff,
    .rainbow_party = true,
    .anim_speed = 5
};

// Colors (Now using config)
#define COL_SKIN_IDLE   (ChameleonAnim::config.color_idle)
#define COL_SKIN_ATTACK (ChameleonAnim::config.color_attack)
#define COL_SKIN_SUCCESS (ChameleonAnim::config.color_success)
#define COL_SKIN_ERROR  (ChameleonAnim::config.color_error)
#define COL_SKIN_SNEAKY (ChameleonAnim::config.color_sneaky)

// Fixed colors
#define COL_TONGUE      0xff5555 
#define COL_EYE_BG      0xffffff
#define COL_PUPIL       0x000000

#include <SD.h>
#include <ArduinoJson.h>

void ChameleonAnim::loadConfig() {
    if (SD.exists("/config/chameleon.json")) {
        File f = SD.open("/config/chameleon.json", FILE_READ);
        if (f) {
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, f);
            if (!error) {
                config.color_idle = doc["idle"] | 0x4ecdc4;
                config.color_attack = doc["attack"] | 0xe94560;
                config.color_success = doc["success"] | 0x00ff00;
                config.color_error = doc["error"] | 0xff0000;
                config.color_sneaky = doc["sneaky"] | 0x222222;
                config.color_eating = doc["eating"] | 0xffa500;
                config.color_party  = doc["party"] | 0xff00ff;
                config.rainbow_party = doc["rainbow"] | true;
                config.anim_speed = doc["speed"] | 5;
                Serial.println("[ANIM] Config loaded");
            }
            f.close();
        }
    }
}

void ChameleonAnim::saveConfig() {
    File f = SD.open("/config/chameleon.json", FILE_WRITE);
    if (f) {
        JsonDocument doc;
        doc["idle"] = config.color_idle;
        doc["attack"] = config.color_attack;
        doc["success"] = config.color_success;
        doc["error"] = config.color_error;
        doc["sneaky"] = config.color_sneaky;
        doc["eating"] = config.color_eating;
        doc["party"] = config.color_party;
        doc["rainbow"] = config.rainbow_party;
        doc["speed"] = config.anim_speed;
        serializeJson(doc, f);
        f.close();
        Serial.println("[ANIM] Config saved");
    }
}

void ChameleonAnim::setConfig(const ChameleonConfig& cfg) {
    config = cfg;
    update_colors();
}

ChameleonConfig ChameleonAnim::getConfig() {
    return config;
}

void ChameleonAnim::init(lv_obj_t* parent, int x, int y) {
    loadConfig(); // Load on init
    stop(); // Ensure clean state
    stop(); // Ensure clean state

    container = lv_obj_create(parent);
    lv_obj_set_pos(container, x, y);
    lv_obj_set_size(container, 120, 100);
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);

    // Body (Curved arc)
    body = lv_obj_create(container);
    lv_obj_set_size(body, 60, 40);
    lv_obj_set_pos(body, 30, 40);
    lv_obj_set_style_radius(body, 20, 0); // Pill shape
    lv_obj_set_style_bg_color(body, lv_color_hex(COL_SKIN_IDLE), 0);
    lv_obj_set_style_border_width(body, 0, 0);

    // Tongue (Hidden by default, behind head)
    tongue = lv_obj_create(container);
    lv_obj_set_size(tongue, 10, 5);
    lv_obj_set_pos(tongue, 90, 45); // Mouth pos
    lv_obj_set_style_radius(tongue, 3, 0);
    lv_obj_set_style_bg_color(tongue, lv_color_hex(COL_TONGUE), 0);
    lv_obj_set_style_border_width(tongue, 0, 0);
    lv_obj_move_background(tongue); // Send to back so it comes out of mouth


    // Head
    head = lv_obj_create(container);
    lv_obj_set_size(head, 50, 45);
    lv_obj_set_pos(head, 65, 25);
    lv_obj_set_style_radius(head, 25, 0);
    lv_obj_set_style_bg_color(head, lv_color_hex(COL_SKIN_IDLE), 0);
    lv_obj_set_style_border_width(head, 0, 0);
    
    // Tail (Simulated with ARC)
    // For simplicity using a curled object or just another circle for now
    tail = lv_obj_create(container);
    lv_obj_set_size(tail, 30, 30);
    lv_obj_set_pos(tail, 10, 50);
    lv_obj_set_style_radius(tail, 15, 0);
    lv_obj_set_style_bg_color(tail, lv_color_hex(COL_SKIN_IDLE), 0);
    lv_obj_set_style_border_width(tail, 0, 0);

    // Eyes (White Sclera)
    eye_left = lv_obj_create(head);
    lv_obj_set_size(eye_left, 16, 16);
    lv_obj_set_pos(eye_left, 5, 10);
    lv_obj_set_style_radius(eye_left, 8, 0);
    lv_obj_set_style_bg_color(eye_left, lv_color_hex(COL_EYE_BG), 0);
    lv_obj_set_style_border_width(eye_left, 0, 0);

    eye_right = lv_obj_create(head);
    lv_obj_set_size(eye_right, 16, 16);
    lv_obj_set_pos(eye_right, 28, 10);
    lv_obj_set_style_radius(eye_right, 8, 0);
    lv_obj_set_style_bg_color(eye_right, lv_color_hex(COL_EYE_BG), 0);
    lv_obj_set_style_border_width(eye_right, 0, 0);

    // Pupils
    pupil_left = lv_obj_create(eye_left);
    lv_obj_set_size(pupil_left, 6, 6);
    lv_obj_center(pupil_left);
    lv_obj_set_style_radius(pupil_left, 3, 0);
    lv_obj_set_style_bg_color(pupil_left, lv_color_hex(COL_PUPIL), 0);
    lv_obj_set_style_border_width(pupil_left, 0, 0);

    pupil_right = lv_obj_create(eye_right);
    lv_obj_set_size(pupil_right, 6, 6);
    lv_obj_center(pupil_right);
    lv_obj_set_style_radius(pupil_right, 3, 0);
    lv_obj_set_style_bg_color(pupil_right, lv_color_hex(COL_PUPIL), 0);
    lv_obj_set_style_border_width(pupil_right, 0, 0);

    // Start animation timer
    anim_timer = lv_timer_create(anim_cb, 50, NULL); // 20 FPS
}

void ChameleonAnim::setMood(ChameleonMood mood) {
    if (mood != currentMood) {
        currentMood = mood;
        // Play sound based on mood
        if (mood == MOOD_SUCCESS) SoundManager::playChirpHappy();
        else if (mood == MOOD_EATING) SoundManager::playChirpEating();
        else if (mood == MOOD_ATTACK) SoundManager::playChirpAngry();
        else if (mood == MOOD_EXCITED) SoundManager::playLevelUp(); // Party!
        
        update_colors();
    }
}

void ChameleonAnim::update_colors() {
    uint32_t color = COL_SKIN_IDLE;
    switch(currentMood) {
        case MOOD_ATTACK: color = COL_SKIN_ATTACK; break;
        case MOOD_SUCCESS: color = COL_SKIN_SUCCESS; break;
        case MOOD_ERROR: color = COL_SKIN_ERROR; break;
        case MOOD_SNEAKY: color = COL_SKIN_SNEAKY; break;
        case MOOD_SLEEP: color = 0x555588; break; // Sleepy blue
        default: color = COL_SKIN_IDLE; break;
    }
    
    // reset tongue
    if(tongue) {
        lv_obj_set_width(tongue, 10);
        lv_obj_set_x(tongue, 90);
    }
    
    // For EXCITED, color handled in anim_cb
    if (currentMood != MOOD_EXCITED) {
        if(head) lv_obj_set_style_bg_color(head, lv_color_hex(color), 0);
        if(body) lv_obj_set_style_bg_color(body, lv_color_hex(color), 0);
        if(tail) lv_obj_set_style_bg_color(tail, lv_color_hex(color), 0);
    }
}

void ChameleonAnim::anim_cb(lv_timer_t* timer) {
    static float t = 0;
    t += 0.1;
    
    // Breathing effect (scale body slightly)
    float breath = 1.0f + 0.05f * sin(t);
    // Note: LVGL scale might be complex on basic objects without image transform, 
    // so we skip scale and do slight position bobbing
    int y_offset = (int)(2 * sin(t));
    if(head) lv_obj_set_y(head, 25 + y_offset);
    if(body) lv_obj_set_y(body, 40 + y_offset);
    if(tail) lv_obj_set_y(tail, 50 + y_offset);

    // Eye movement logic
    static int eye_timer = 0;
    static int target_x_l = 0, target_y_l = 0;
    static int target_x_r = 0, target_y_r = 0;
    
    // Rainbow Effect for EXCITED
    if (currentMood == MOOD_EXCITED) {
        if (config.rainbow_party) {
            // HSV Rainbow
            static int hue = 0;
            hue = (hue + config.anim_speed) % 360; // Use Speed
            lv_color_t rainbow = lv_color_hsv_to_rgb(hue, 100, 100);
            if(head) lv_obj_set_style_bg_color(head, rainbow, 0);
            if(body) lv_obj_set_style_bg_color(body, rainbow, 0);
            if(tail) lv_obj_set_style_bg_color(tail, rainbow, 0);
        } else {
             // Static Party Color
             if(head) lv_obj_set_style_bg_color(head, lv_color_hex(config.color_party), 0);
             if(body) lv_obj_set_style_bg_color(body, lv_color_hex(config.color_party), 0);
             if(tail) lv_obj_set_style_bg_color(tail, lv_color_hex(config.color_party), 0);
        }
        
        // Fast bouncing
        y_offset = (int)(config.anim_speed * sin(t*3));
    }
    
    // Tongue Animation for EATING
    if (currentMood == MOOD_EATING && tongue) {
        // Extend and retract
        float tongue_phase = fmod(t * 2.0f, 6.28f); // faster cycle
        // Extend: 0 to 60px
        int ext = 0;
        if (tongue_phase < 3.14f) {
            ext = (int)(60 * sin(tongue_phase)); 
        }
        lv_obj_set_width(tongue, 10 + ext);
    }

    if (currentMood == MOOD_ATTACK) {
        // Crazy eyes
        target_x_l = (int)(4 * sin(t*2));
        target_y_l = (int)(4 * cos(t*3));
        target_x_r = (int)(4 * sin(t*3));
        target_y_r = (int)(4 * cos(t*2));
    } else if (currentMood == MOOD_CONFUSED) {
        // Spin eyes opposite
        target_x_l = (int)(3 * sin(t));
        target_y_l = (int)(3 * cos(t));
        target_x_r = (int)(3 * sin(-t)); // Opposite dir
        target_y_r = (int)(3 * cos(-t));
    } else if (currentMood == MOOD_SLEEP) {
        // Eyes closed (simulated by looking down/hidden)
        // Or we could resize eyes, but let's just look down
        target_x_l = 0; target_y_l = 5;
        target_x_r = 0; target_y_r = 5;
        // Slow breath
        lv_obj_set_y(body, 40 + (int)(1 * sin(t*0.5))); 
    } else if (currentMood == MOOD_IDLE || currentMood == MOOD_SNEAKY) {
        // Random look
        if (eye_timer++ > (currentMood == MOOD_SNEAKY ? 100 : 50)) { // Slower if sneaky
            eye_timer = 0;
            target_x_l = random(-3, 4);
            target_y_l = random(-3, 4);
            target_x_r = random(-3, 4);
            target_y_r = random(-3, 4);
        }
    } else {
        // Centered
        target_x_l = 0; target_y_l = 0;
        target_x_r = 0; target_y_r = 0;
    }

    // Smooth lerp pupil
    // (Simple direct set for now to save CPU)
    if(pupil_left) lv_obj_align(pupil_left, LV_ALIGN_CENTER, target_x_l, target_y_l);
    if(pupil_right) lv_obj_align(pupil_right, LV_ALIGN_CENTER, target_x_r, target_y_r);
}

void ChameleonAnim::loop() {
    // Timer handles logic
}

void ChameleonAnim::stop() {
    if (anim_timer) {
        lv_timer_del(anim_timer);
        anim_timer = nullptr;
    }
    container = nullptr;
    head = nullptr;
    body = nullptr;
    head = nullptr;
    body = nullptr;
    tail = nullptr;
    tongue = nullptr;
    eye_left = nullptr;
    eye_right = nullptr;
    pupil_left = nullptr;
    pupil_right = nullptr;
}
