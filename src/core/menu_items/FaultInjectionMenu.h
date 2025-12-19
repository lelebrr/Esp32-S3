/**
 * @file FaultInjectionMenu.h
 * @brief Pocket Glitcher Pro - Touchscreen Menu Interface
 * @author Lele Origin Team
 */

#ifndef __FAULT_INJECTION_MENU_H__
#define __FAULT_INJECTION_MENU_H__

#include <lvgl.h>
#include <MenuItemInterface.h>
#include "modules/fault_injection/fault_injection.h"

class FaultInjectionMenu : public MenuItemInterface {
public:
    FaultInjectionMenu();
    ~FaultInjectionMenu();

    // MenuItemInterface implementation
    // String getName() override { return "Fault Injection"; } // Removed: Not virtual in base
    void optionsMenu() override;
    // void draw() override; // Deprecated/Incorrect override signature usually
    void drawIcon(float scale) override;
    void drawIconImg() override;
    bool getTheme() override;

private:
    // Menu states
    enum MenuState {
        STATE_MAIN = 0,
        STATE_TARGET_SELECT,
        STATE_CONFIGURE,
        STATE_ARM_SYSTEM,
        STATE_GLITCH_ACTIVE,
        STATE_RESULTS
    };

    MenuState currentState;
    GlitchTarget selectedTarget;
    GlitchConfig currentConfig;
    GlitchResult lastResult;

    // UI elements
    lv_obj_t* mainContainer;
    lv_obj_t* targetList;
    lv_obj_t* configPanel;
    lv_obj_t* statusLabel;
    lv_obj_t* progressBar;
    lv_obj_t* resultDisplay;

    // Menu navigation
    void showMainMenu();
    void showTargetSelection();
    void showConfiguration();
    void showArmSystem();
    void showGlitchActive();
    void showResults();

    // Event handlers
    static void targetSelectEvent(lv_event_t* e);
    static void configEvent(lv_event_t* e);
    static void armEvent(lv_event_t* e);
    static void glitchEvent(lv_event_t* e);
    static void backEvent(lv_event_t* e);

    // UI helpers
    void createTargetButtons();
    void createConfigSliders();
    void updateStatusDisplay();
    void updateProgressBar(int progress);
    void showResultScreen();

    // Configuration helpers
    void setDefaultConfigForTarget(GlitchTarget target);
    String getTargetDescription(GlitchTarget target);
    String getGlitchTypeName(GlitchType type);
    String getTriggerTypeName(TriggerType type);
};

// Global instance
extern FaultInjectionMenu faultInjectionMenu;

#endif // __FAULT_INJECTION_MENU_H__
