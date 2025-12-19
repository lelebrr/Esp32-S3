#pragma once
#include <Arduino.h>
#include <lvgl.h>

class PanicMode {
public:
    static PanicMode& getInstance() {
        static PanicMode instance;
        return instance;
    }

    void init();
    void update();
    void trigger();
    bool isActive() const { return _active; }
    void checkTrigger(int touchX, int touchY);

private:
    PanicMode() : _active(false), _lastTouch(0) {}
    bool _active;
    unsigned long _lastTouch;
    int _touchCount;
    
    // UI Elements
    lv_obj_t* _screen;
    lv_obj_t* _labelTime;
    lv_obj_t* _labelDate;
    
    void drawFakeClock();
    void exitPanic();
};
