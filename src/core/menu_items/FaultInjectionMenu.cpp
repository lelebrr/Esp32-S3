/**
 * @file FaultInjectionMenu.cpp
 * @brief Pocket Glitcher Pro - Touchscreen Menu Implementation
 * @author Lele Origin Team
 */

#include "FaultInjectionMenu.h"
#include <globals.h>
#include <core/display.h>
#include <core/mykeyboard.h>

// Global instance
FaultInjectionMenu faultInjectionMenu;

FaultInjectionMenu::FaultInjectionMenu()
    : MenuItemInterface("Fault Injection"), currentState(STATE_MAIN), selectedTarget(TARGET_SWITCH_RCM),
      mainContainer(nullptr), targetList(nullptr), configPanel(nullptr),
      statusLabel(nullptr), progressBar(nullptr), resultDisplay(nullptr) {
}

FaultInjectionMenu::~FaultInjectionMenu() {
    // Cleanup will be handled by LVGL
}

void FaultInjectionMenu::optionsMenu() {
    // Initialize fault injection system if not already done
    if (!initializeFaultInjectionSystem()) {
        displayError("Failed to initialize Fault Injection system", true);
        return;
    }

    // Create main container
    mainContainer = lv_obj_create(NULL);
    lv_scr_load(mainContainer);

    // Start with main menu
    showMainMenu();

    // Menu loop
    while (true) {
        lv_task_handler();
        delay(5);

        // Check for exit condition
        if (returnToMenu) {
            returnToMenu = false;
            break;
        }
    }

    // Cleanup
    lv_obj_del(mainContainer);
    mainContainer = nullptr;
}

void FaultInjectionMenu::drawIcon(float scale) {
    clearIconArea();
    tft.fillRoundRect(iconCenterX - 20 * scale, iconCenterY - 15 * scale, 40 * scale, 30 * scale, 4 * scale, lv_color_make(255, 69, 0).full); // Orange
    tft.drawCentreString("GLITCH", iconCenterX, iconCenterY - 5 * scale, 1);
}

void FaultInjectionMenu::drawIconImg() {
    // Check for icon path
    // if (leleConfig.theme.paths.faultInjection != "") ...
    // Using generic implementation for now
}

bool FaultInjectionMenu::getTheme() { 
    return false; // Default
}

void FaultInjectionMenu::showMainMenu() {
    currentState = STATE_MAIN;

    // Clear container
    lv_obj_clean(mainContainer);

    // Title
    lv_obj_t* title = lv_label_create(mainContainer);
    lv_label_set_text(title, "POCKET GLITCHER PRO");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFF6B35), 0);

    // Subtitle
    lv_obj_t* subtitle = lv_label_create(mainContainer);
    lv_label_set_text(subtitle, "Fault Injection System");
    lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 35);
    lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_14, 0);

    // Status display
    statusLabel = lv_label_create(mainContainer);
    lv_obj_align(statusLabel, LV_ALIGN_TOP_LEFT, 10, 70);
    lv_obj_set_size(statusLabel, lv_pct(80), 60);
    updateStatusDisplay();

    // Main buttons
    lv_obj_t* btnSelectTarget = lv_btn_create(mainContainer);
    lv_obj_set_size(btnSelectTarget, 140, 50);
    lv_obj_align(btnSelectTarget, LV_ALIGN_LEFT_MID, 10, -20);
    lv_obj_t* lblSelectTarget = lv_label_create(btnSelectTarget);
    lv_label_set_text(lblSelectTarget, "Select Target");
    lv_obj_add_event_cb(btnSelectTarget, [](lv_event_t* e) {
        faultInjectionMenu.showTargetSelection();
    }, LV_EVENT_CLICKED, nullptr);

    lv_obj_t* btnConfigure = lv_btn_create(mainContainer);
    lv_obj_set_size(btnConfigure, 140, 50);
    lv_obj_align(btnConfigure, LV_ALIGN_RIGHT_MID, -10, -20);
    lv_obj_t* lblConfigure = lv_label_create(btnConfigure);
    lv_label_set_text(lblConfigure, "Configure");
    lv_obj_add_event_cb(btnConfigure, [](lv_event_t* e) {
        faultInjectionMenu.showConfiguration();
    }, LV_EVENT_CLICKED, nullptr);

    // Bottom buttons
    lv_obj_t* btnArm = lv_btn_create(mainContainer);
    lv_obj_set_size(btnArm, 100, 40);
    lv_obj_align(btnArm, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_t* lblArm = lv_label_create(btnArm);
    lv_label_set_text(lblArm, "ARM");
    lv_obj_set_style_bg_color(btnArm, lv_color_hex(0xFF4500), 0);
    lv_obj_add_event_cb(btnArm, [](lv_event_t* e) {
        faultInjectionMenu.showArmSystem();
    }, LV_EVENT_CLICKED, nullptr);

    lv_obj_t* btnGlitch = lv_btn_create(mainContainer);
    lv_obj_set_size(btnGlitch, 100, 40);
    lv_obj_align(btnGlitch, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_t* lblGlitch = lv_label_create(btnGlitch);
    lv_label_set_text(lblGlitch, "GLITCH!");
    lv_obj_set_style_bg_color(btnGlitch, lv_color_hex(0xFF0000), 0);
    lv_obj_add_event_cb(btnGlitch, [](lv_event_t* e) {
        faultInjectionMenu.showGlitchActive();
    }, LV_EVENT_CLICKED, nullptr);

    lv_obj_t* btnBack = lv_btn_create(mainContainer);
    lv_obj_set_size(btnBack, 100, 40);
    lv_obj_align(btnBack, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_t* lblBack = lv_label_create(btnBack);
    lv_label_set_text(lblBack, "Back");
    lv_obj_add_event_cb(btnBack, [](lv_event_t* e) {
        returnToMenu = true;
    }, LV_EVENT_CLICKED, nullptr);

    // Emergency stop button (small, red)
    lv_obj_t* btnStop = lv_btn_create(mainContainer);
    lv_obj_set_size(btnStop, 60, 30);
    lv_obj_align(btnStop, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_t* lblStop = lv_label_create(btnStop);
    lv_label_set_text(lblStop, "STOP");
    lv_obj_set_style_bg_color(btnStop, lv_color_hex(0x8B0000), 0);
    lv_obj_set_style_text_font(lblStop, &lv_font_montserrat_12, 0);
    lv_obj_add_event_cb(btnStop, [](lv_event_t* e) {
        if (faultInjectionManager) {
            faultInjectionManager->emergencyStop();
            faultInjectionMenu.updateStatusDisplay();
        }
    }, LV_EVENT_CLICKED, nullptr);
}

void FaultInjectionMenu::showTargetSelection() {
    currentState = STATE_TARGET_SELECT;

    // Clear container
    lv_obj_clean(mainContainer);

    // Title
    lv_obj_t* title = lv_label_create(mainContainer);
    lv_label_set_text(title, "SELECT TARGET");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);

    // Target list container
    lv_obj_t* listContainer = lv_obj_create(mainContainer);
    lv_obj_set_size(listContainer, lv_pct(90), lv_pct(70));
    lv_obj_align(listContainer, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_set_style_bg_color(listContainer, lv_color_hex(0x333333), 0);

    // Create target buttons
    createTargetButtons();

    // Back button
    lv_obj_t* btnBack = lv_btn_create(mainContainer);
    lv_obj_set_size(btnBack, 80, 40);
    lv_obj_align(btnBack, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_t* lblBack = lv_label_create(btnBack);
    lv_label_set_text(lblBack, "Back");
    lv_obj_add_event_cb(btnBack, [](lv_event_t* e) {
        faultInjectionMenu.showMainMenu();
    }, LV_EVENT_CLICKED, nullptr);
}

void FaultInjectionMenu::createTargetButtons() {
    const char* targetNames[] = {
        "Nintendo Switch RCM",
        "iPhone Checkm8",
        "Car Keyless Entry",
        "Smart Meter",
        "RFID HID Prox",
        "Custom Target"
    };

    lv_obj_t* listContainer = lv_obj_get_child(mainContainer, 1); // Second child is the list container

    for (int i = 0; i < 6; i++) {
        lv_obj_t* btn = lv_btn_create(listContainer);
        lv_obj_set_size(btn, lv_pct(90), 45);
        lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, i * 50 + 5);

        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text(label, targetNames[i]);
        lv_obj_center(label);

        // Store target index in user data
        lv_obj_set_user_data(btn, (void*)i);

        lv_obj_add_event_cb(btn, [](lv_event_t* e) {
            lv_obj_t* btn = lv_event_get_target(e);
            int targetIndex = (int)lv_obj_get_user_data(btn);

            faultInjectionMenu.selectedTarget = (GlitchTarget)targetIndex;
            faultInjectionMenu.setDefaultConfigForTarget(faultInjectionMenu.selectedTarget);
            faultInjectionMenu.showMainMenu();
        }, LV_EVENT_CLICKED, nullptr);
    }
}

void FaultInjectionMenu::showConfiguration() {
    currentState = STATE_CONFIGURE;

    // Clear container
    lv_obj_clean(mainContainer);

    // Title
    lv_obj_t* title = lv_label_create(mainContainer);
    lv_label_set_text(title, "CONFIGURE GLITCH");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);

    // Configuration panel
    configPanel = lv_obj_create(mainContainer);
    lv_obj_set_size(configPanel, lv_pct(90), lv_pct(70));
    lv_obj_align(configPanel, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_set_style_bg_color(configPanel, lv_color_hex(0x333333), 0);

    // Create configuration controls
    createConfigSliders();

    // Save button
    lv_obj_t* btnSave = lv_btn_create(mainContainer);
    lv_obj_set_size(btnSave, 80, 40);
    lv_obj_align(btnSave, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_t* lblSave = lv_label_create(btnSave);
    lv_label_set_text(lblSave, "Save");
    lv_obj_set_style_bg_color(btnSave, lv_color_hex(0x228B22), 0);
    lv_obj_add_event_cb(btnSave, [](lv_event_t* e) {
        if (faultInjectionManager) {
            faultInjectionManager->configureGlitch(faultInjectionMenu.currentConfig);
            faultInjectionMenu.showMainMenu();
        }
    }, LV_EVENT_CLICKED, nullptr);

    // Back button
    lv_obj_t* btnBack = lv_btn_create(mainContainer);
    lv_obj_set_size(btnBack, 80, 40);
    lv_obj_align(btnBack, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_t* lblBack = lv_label_create(btnBack);
    lv_label_set_text(lblBack, "Back");
    lv_obj_add_event_cb(btnBack, [](lv_event_t* e) {
        faultInjectionMenu.showMainMenu();
    }, LV_EVENT_CLICKED, nullptr);
}

void FaultInjectionMenu::createConfigSliders() {
    // Target info
    lv_obj_t* targetInfo = lv_label_create(configPanel);
    lv_label_set_text_fmt(targetInfo, "Target: %s", getTargetDescription(selectedTarget).c_str());
    lv_obj_align(targetInfo, LV_ALIGN_TOP_LEFT, 10, 10);

    // Pulse width slider
    lv_obj_t* pulseLabel = lv_label_create(configPanel);
    lv_label_set_text(pulseLabel, "Pulse Width");
    lv_obj_align(pulseLabel, LV_ALIGN_TOP_LEFT, 10, 40);

    lv_obj_t* pulseSlider = lv_slider_create(configPanel);
    lv_obj_set_size(pulseSlider, lv_pct(70), 20);
    lv_obj_align(pulseSlider, LV_ALIGN_TOP_LEFT, 10, 60);
    lv_slider_set_range(pulseSlider, 50, 500);
    lv_slider_set_value(pulseSlider, currentConfig.pulseWidth, LV_ANIM_OFF);

    // Delay slider
    lv_obj_t* delayLabel = lv_label_create(configPanel);
    lv_label_set_text(delayLabel, "Delay (µs)");
    lv_obj_align(delayLabel, LV_ALIGN_TOP_LEFT, 10, 90);

    lv_obj_t* delaySlider = lv_slider_create(configPanel);
    lv_obj_set_size(delaySlider, lv_pct(70), 20);
    lv_obj_align(delaySlider, LV_ALIGN_TOP_LEFT, 10, 110);
    lv_slider_set_range(delaySlider, 1, 1000);
    lv_slider_set_value(delaySlider, currentConfig.delay, LV_ANIM_OFF);

    // Intensity slider
    lv_obj_t* intensityLabel = lv_label_create(configPanel);
    lv_label_set_text(intensityLabel, "Intensity (%)");
    lv_obj_align(intensityLabel, LV_ALIGN_TOP_LEFT, 10, 140);

    lv_obj_t* intensitySlider = lv_slider_create(configPanel);
    lv_obj_set_size(intensitySlider, lv_pct(70), 20);
    lv_obj_align(intensitySlider, LV_ALIGN_TOP_LEFT, 10, 160);
    lv_slider_set_range(intensitySlider, 0, 100);
    lv_slider_set_value(intensitySlider, currentConfig.intensity, LV_ANIM_OFF);

    // Glitch type selector
    lv_obj_t* typeLabel = lv_label_create(configPanel);
    lv_label_set_text(typeLabel, "Glitch Type");
    lv_obj_align(typeLabel, LV_ALIGN_TOP_LEFT, 10, 190);

    // Add dropdown or buttons for glitch type selection
    // (Simplified for this implementation)
}

void FaultInjectionMenu::showArmSystem() {
    currentState = STATE_ARM_SYSTEM;

    // Clear container
    lv_obj_clean(mainContainer);

    // Warning title
    lv_obj_t* title = lv_label_create(mainContainer);
    lv_label_set_text(title, "⚠️ ARM SYSTEM");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFA500), 0);

    // Warning message
    lv_obj_t* warning = lv_label_create(mainContainer);
    lv_label_set_text(warning,
        "WARNING: Arming the system enables\n"
        "high-voltage glitch circuits.\n"
        "Ensure proper target connection!\n\n"
        "Target: High voltage risk!\n"
        "Operator: Electrical shock risk!");
    lv_obj_align(warning, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_color(warning, lv_color_hex(0xFF0000), 0);

    // Arm buttons
    lv_obj_t* btnArmCrowbar = lv_btn_create(mainContainer);
    lv_obj_set_size(btnArmCrowbar, 120, 50);
    lv_obj_align(btnArmCrowbar, LV_ALIGN_LEFT_MID, 20, -20);
    lv_obj_t* lblArmCrowbar = lv_label_create(btnArmCrowbar);
    lv_label_set_text(lblArmCrowbar, "ARM CROWBAR");
    lv_obj_set_style_bg_color(btnArmCrowbar, lv_color_hex(0xFF4500), 0);
    lv_obj_add_event_cb(btnArmCrowbar, [](lv_event_t* e) {
        if (faultInjectionManager && faultInjectionManager->getCrowbar()) {
            faultInjectionManager->getCrowbar()->setArmed(true);
            faultInjectionMenu.updateStatusDisplay();
        }
    }, LV_EVENT_CLICKED, nullptr);

    lv_obj_t* btnArmLaser = lv_btn_create(mainContainer);
    lv_obj_set_size(btnArmLaser, 120, 50);
    lv_obj_align(btnArmLaser, LV_ALIGN_RIGHT_MID, -20, -20);
    lv_obj_t* lblArmLaser = lv_label_create(btnArmLaser);
    lv_label_set_text(lblArmLaser, "ARM LASER");
    lv_obj_set_style_bg_color(btnArmLaser, lv_color_hex(0xFF4500), 0);
    lv_obj_add_event_cb(btnArmLaser, [](lv_event_t* e) {
        if (faultInjectionManager && faultInjectionManager->getLaser()) {
            faultInjectionManager->getLaser()->setArmed(true);
            faultInjectionMenu.updateStatusDisplay();
        }
    }, LV_EVENT_CLICKED, nullptr);

    // System arm button
    lv_obj_t* btnArmSystem = lv_btn_create(mainContainer);
    lv_obj_set_size(btnArmSystem, 140, 60);
    lv_obj_align(btnArmSystem, LV_ALIGN_BOTTOM_MID, 0, -80);
    lv_obj_t* lblArmSystem = lv_label_create(btnArmSystem);
    lv_label_set_text(lblArmSystem, "ARM SYSTEM");
    lv_obj_set_style_bg_color(btnArmSystem, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_text_font(lblArmSystem, &lv_font_montserrat_16, 0);
    lv_obj_add_event_cb(btnArmSystem, [](lv_event_t* e) {
        if (faultInjectionManager) {
            faultInjectionManager->armSystem(true);
            faultInjectionMenu.updateStatusDisplay();
        }
    }, LV_EVENT_CLICKED, nullptr);

    // Back button
    lv_obj_t* btnBack = lv_btn_create(mainContainer);
    lv_obj_set_size(btnBack, 80, 40);
    lv_obj_align(btnBack, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_t* lblBack = lv_label_create(btnBack);
    lv_label_set_text(lblBack, "Back");
    lv_obj_add_event_cb(btnBack, [](lv_event_t* e) {
        faultInjectionMenu.showMainMenu();
    }, LV_EVENT_CLICKED, nullptr);
}

void FaultInjectionMenu::showGlitchActive() {
    currentState = STATE_GLITCH_ACTIVE;

    // Clear container
    lv_obj_clean(mainContainer);

    // Active glitch display
    lv_obj_t* title = lv_label_create(mainContainer);
    lv_label_set_text(title, "⚡ GLITCH ACTIVE");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFF0000), 0);

    // Progress bar
    progressBar = lv_bar_create(mainContainer);
    lv_obj_set_size(progressBar, lv_pct(80), 20);
    lv_obj_align(progressBar, LV_ALIGN_TOP_MID, 0, 50);
    lv_bar_set_range(progressBar, 0, 100);
    lv_bar_set_value(progressBar, 0, LV_ANIM_OFF);

    // Status text
    lv_obj_t* statusText = lv_label_create(mainContainer);
    lv_label_set_text(statusText, "Preparing glitch...");
    lv_obj_align(statusText, LV_ALIGN_TOP_MID, 0, 80);

    // Execute glitch in background
    if (faultInjectionManager) {
        // Start glitch
        lastResult = faultInjectionManager->performGlitch();

        // Update progress and show results
        updateProgressBar(100);
        showResults();
    } else {
        lv_label_set_text(statusText, "Fault Injection System not initialized!");
    }
}

void FaultInjectionMenu::showResults() {
    currentState = STATE_RESULTS;

    // Clear container
    lv_obj_clean(mainContainer);

    // Result title
    lv_obj_t* title = lv_label_create(mainContainer);
    if (lastResult.success) {
        lv_label_set_text(title, "✅ GLITCH SUCCESS!");
        lv_obj_set_style_text_color(title, lv_color_hex(0x00FF00), 0);
    } else {
        lv_label_set_text(title, "❌ GLITCH FAILED");
        lv_obj_set_style_text_color(title, lv_color_hex(0xFF0000), 0);
    }
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);

    // Result details
    lv_obj_t* resultText = lv_label_create(mainContainer);
    String resultStr = "Execution Time: " + String(lastResult.executionTime) + "ms\n";
    resultStr += "Pulse Width: " + String(lastResult.pulseWidthUsed) + "\n";
    resultStr += "Delay: " + String(lastResult.delayUsed) + "µs\n";
    resultStr += "Attempts: " + String(lastResult.attempts) + "\n";

    if (!lastResult.success) {
        resultStr += "Error: " + lastResult.errorMessage + "\n";
    }

    lv_label_set_text(resultText, resultStr.c_str());
    lv_obj_align(resultText, LV_ALIGN_TOP_LEFT, 10, 50);
    lv_obj_set_size(resultText, lv_pct(90), 120);

    // Action buttons
    lv_obj_t* btnRetry = lv_btn_create(mainContainer);
    lv_obj_set_size(btnRetry, 100, 50);
    lv_obj_align(btnRetry, LV_ALIGN_BOTTOM_LEFT, 20, -10);
    lv_obj_t* lblRetry = lv_label_create(btnRetry);
    lv_label_set_text(lblRetry, "Retry");
    lv_obj_set_style_bg_color(btnRetry, lv_color_hex(0xFFA500), 0);
    lv_obj_add_event_cb(btnRetry, [](lv_event_t* e) {
        faultInjectionMenu.showGlitchActive();
    }, LV_EVENT_CLICKED, nullptr);

    lv_obj_t* btnNewTarget = lv_btn_create(mainContainer);
    lv_obj_set_size(btnNewTarget, 100, 50);
    lv_obj_align(btnNewTarget, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_t* lblNewTarget = lv_label_create(btnNewTarget);
    lv_label_set_text(lblNewTarget, "New Target");
    lv_obj_add_event_cb(btnNewTarget, [](lv_event_t* e) {
        faultInjectionMenu.showTargetSelection();
    }, LV_EVENT_CLICKED, nullptr);

    lv_obj_t* btnMainMenu = lv_btn_create(mainContainer);
    lv_obj_set_size(btnMainMenu, 100, 50);
    lv_obj_align(btnMainMenu, LV_ALIGN_BOTTOM_RIGHT, -20, -10);
    lv_obj_t* lblMainMenu = lv_label_create(btnMainMenu);
    lv_label_set_text(lblMainMenu, "Main Menu");
    lv_obj_add_event_cb(btnMainMenu, [](lv_event_t* e) {
        faultInjectionMenu.showMainMenu();
    }, LV_EVENT_CLICKED, nullptr);
}

void FaultInjectionMenu::updateStatusDisplay() {
    if (!statusLabel) return;

    String status = getFaultInjectionSystemInfo();
    lv_label_set_text(statusLabel, status.c_str());
}

void FaultInjectionMenu::updateProgressBar(int progress) {
    if (progressBar) {
        lv_bar_set_value(progressBar, progress, LV_ANIM_ON);
    }
}

void FaultInjectionMenu::setDefaultConfigForTarget(GlitchTarget target) {
    const TargetTiming* timing = nullptr;
    if (faultInjectionManager) {
        timing = faultInjectionManager->getTargetTiming(target);
    }

    if (timing) {
        currentConfig.target = target;
        currentConfig.delay = timing->optimalDelay;
        currentConfig.pulseWidth = timing->optimalPulse;
    } else {
        // Default config
        currentConfig.target = target;
        currentConfig.delay = 100000;
        currentConfig.pulseWidth = 100000;
    }

    currentConfig.type = GLITCH_CROWBAR;
    currentConfig.trigger = TRIGGER_MANUAL;
    currentConfig.intensity = 100;
    currentConfig.autoRetry = true;
    currentConfig.maxRetries = 3;
}

String FaultInjectionMenu::getTargetDescription(GlitchTarget target) {
    if (faultInjectionManager) {
        const TargetTiming* timing = faultInjectionManager->getTargetTiming(target);
        if (timing) {
            return String(timing->name);
        }
    }

    switch (target) {
        case TARGET_SWITCH_RCM: return "Nintendo Switch RCM";
        case TARGET_IPHONE_CHECKM8: return "iPhone Checkm8";
        case TARGET_CAR_KEYLESS: return "Car Keyless Entry";
        case TARGET_SMART_METER: return "Smart Meter";
        case TARGET_RFID_HID: return "RFID HID Prox";
        case TARGET_CUSTOM: return "Custom Target";
        default: return "Unknown Target";
    }
}

String FaultInjectionMenu::getGlitchTypeName(GlitchType type) {
    switch (type) {
        case GLITCH_CROWBAR: return "Crowbar";
        case GLITCH_LASER: return "Laser";
        case GLITCH_COMBO: return "Combo";
        case GLITCH_RF_TRIGGER: return "RF Trigger";
        default: return "Unknown";
    }
}

String FaultInjectionMenu::getTriggerTypeName(TriggerType type) {
    switch (type) {
        case TRIGGER_MANUAL: return "Manual";
        case TRIGGER_CC1101: return "CC1101";
        case TRIGGER_NRF24: return "NRF24";
        case TRIGGER_AUTO: return "Auto";
        default: return "Unknown";
    }
}
