#include "core/main_menu.h"
#include "core/optimization_manager.h"
#include "core/system/PanicMode.h"
#include "core/persistence.h"
#include "core/stealth.h"
#include <globals.h>
#include <Preferences.h>

#include "core/powerSave.h"
#include "core/serial_commands/cli.h"
#include "core/utils.h"
#include "esp32-hal-psram.h"
#include "esp_task_wdt.h"
#include <functional>
#include <string>
#include <vector>
io_expander ioExpander;
LeleConfig leleConfig;
LeleConfigPins leleConfigPins;

SerialCli serialCli;
USBSerial USBserial;
SerialDevice *serialDevice = &USBserial;

Preferences preferences;
enum PowerMode { ECONOMY = 0, BALANCED = 1, FORCE = 2 };
PowerMode currentPowerMode = BALANCED;

StartupApp startupApp;
MainMenu mainMenu;
SPIClass sdcardSPI;
#ifdef USE_HSPI_PORT
SPIClass CC_NRF_SPI(VSPI);
#else
SPIClass CC_NRF_SPI(HSPI);
#endif

// Navigation Variables
volatile bool NextPress = false;
volatile bool PrevPress = false;
volatile bool UpPress = false;
volatile bool DownPress = false;
volatile bool SelPress = false;
volatile bool EscPress = false;
volatile bool AnyKeyPress = false;
volatile bool NextPagePress = false;
volatile bool PrevPagePress = false;
volatile bool LongPress = false;
volatile bool SerialCmdPress = false;
volatile int forceMenuOption = -1;
volatile uint8_t menuOptionType = 0;
String menuOptionLabel = "";
#ifdef HAS_ENCODER_LED
volatile int EncoderLedChange = 0;
#endif

TouchPoint touchPoint;

keyStroke KeyStroke;

TaskHandle_t xHandle;
void __attribute__((weak)) taskInputHandler(void *parameter) {
    auto timer = millis();
    while (true) {
        checkPowerSaveTime();
        // Sometimes this task run 2 or more times before looptask,
        // and navigation gets stuck, the idea here is run the input detection
        // if AnyKeyPress is false, or rerun if it was not renewed within 75ms (arbitrary)
        // because AnyKeyPress will be true if didn´t passed through a check(bool var)
        if (!AnyKeyPress || millis() - timer > 75) {
            NextPress = false;
            PrevPress = false;
            UpPress = false;
            DownPress = false;
            SelPress = false;
            EscPress = false;
            AnyKeyPress = false;
            SerialCmdPress = false;
            NextPagePress = false;
            PrevPagePress = false;
            touchPoint.pressed = false;
            touchPoint.Clear();
#ifndef USE_TFT_eSPI_TOUCH
            InputHandler();
#endif
            timer = millis();
        }

        // Manual deep sleep trigger (5s hold SEL button)
        static bool longSel = false;
        static unsigned long selPressTime = 0;
        if (SelPress && !longSel) {
            longSel = true;
            selPressTime = millis();
        } else if (longSel && millis() - selPressTime > 5000) {
            esp_sleep_enable_timer_wakeup(60000 * 1000);
            esp_deep_sleep_start();
        } else if (!SelPress) {
            longSel = false;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
// Public Globals Variables
unsigned long previousMillis = millis();
int prog_handler; // 0 - Flash, 1 - LittleFS, 3 - Download
String cachedPassword = "";
bool interpreter_start = false;
bool sdcardMounted = false;
bool gpsConnected = false;

// wifi globals - organizados em namespace para melhor encapsulamento
// wifi globals
bool wifiConnected = false;
bool isWebUIActive = false;
String wifiIP;

bool BLEConnected = false;
bool returnToMenu;
bool isSleeping = false;
bool isScreenOff = false;
bool dimmer = false;
char timeStr[10];
time_t localTime;
struct tm *timeInfo;
#if defined(HAS_RTC)
cplus_RTC _rtc;
RTC_TimeTypeDef _time;
RTC_DateTypeDef _date;
bool clock_set = true;
#else
ESP32Time rtc;
bool clock_set = false;
#endif

std::vector<Option> options;
// Protected global variables
#if defined(HAS_SCREEN)
tft_logger tft = tft_logger(); // Invoke custom library
TFT_eSprite sprite = TFT_eSprite(&tft);
TFT_eSprite draw = TFT_eSprite(&tft);
volatile int tftWidth = TFT_HEIGHT;
#ifdef HAS_TOUCH
volatile int tftHeight =
    TFT_WIDTH - 20; // 20px to draw the TouchFooter(), were the btns are being read in touch devices.
#else
volatile int tftHeight = TFT_WIDTH;
#endif
#else
tft_logger tft;
SerialDisplayClass &sprite = tft;
SerialDisplayClass &draw = tft;
volatile int tftWidth = VECTOR_DISPLAY_DEFAULT_HEIGHT;
volatile int tftHeight = VECTOR_DISPLAY_DEFAULT_WIDTH;
#endif

#include "core/boot_hw_scan.h" // for boot_screen_hw
#include "core/display.h"
#include "core/led_control.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"
#include "core/serialcmds.h"
#include "core/settings.h"
#include "core/wifi/webInterface.h"
#include "core/wifi/wifi_common.h"
#include "modules/bjs_interpreter/interpreter.h" // for JavaScript interpreter
#include "modules/others/audio.h"                // for playAudioFile
#include "modules/rf/rf_utils.h"                 // for initCC1101once
#include <Wire.h>
#include <driver/uart.h>

void showBootMenu() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawCentreString("Power Mode Selection", tftWidth / 2, 10, 2);
    tft.setTextSize(1);
    tft.drawCentreString("Press GPIO0 to enter, timeout 3s", tftWidth / 2, 40, 1);
    pinMode(0, INPUT_PULLUP);
    unsigned long start = millis();
    while (millis() - start < 3000) {
        if (digitalRead(0) == LOW) {
            selectMode();
            return;
        }
        delay(10);
    }
    tft.drawCentreString("Using saved mode", tftWidth / 2, 60, 1);
    delay(1000);
}

void selectMode() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.drawCentreString("Select Mode:", tftWidth / 2, 10, 2);
    tft.setTextSize(1);
    tft.drawCentreString("1: Economy (14 days)", tftWidth / 2, 40, 1);
    tft.drawCentreString("2: Balanced (5 days)", tftWidth / 2, 60, 1);
    tft.drawCentreString("3: Force (8h)", tftWidth / 2, 80, 1);
    tft.drawCentreString("Warning: Force drains battery fast", tftWidth / 2, 100, 1);
    tft.drawCentreString("Use buttons to select", tftWidth / 2, 120, 1);
    pinMode(leleConfigPins.SEL, INPUT_PULLUP);
    pinMode(leleConfigPins.ESC, INPUT_PULLUP);
    pinMode(leleConfigPins.NEXT, INPUT_PULLUP);
    pinMode(leleConfigPins.PREV, INPUT_PULLUP);
    int selected = currentPowerMode + 1;
    while (true) {
        tft.fillRect(0, 140, tftWidth, 20, TFT_BLACK);
        tft.drawCentreString("Selected: " + String(selected), tftWidth / 2, 140, 1);
        if (digitalRead(leleConfigPins.SEL) == LOW) {
            currentPowerMode = (PowerMode)(selected - 1);
            preferences.putUChar("bat_mode", currentPowerMode);
            tft.drawCentreString("Saved!", tftWidth / 2, 160, 1);
            delay(500);
            applyPowerMode();
            return;
        }
        if (digitalRead(leleConfigPins.ESC) == LOW) {
            tft.drawCentreString("Cancelled", tftWidth / 2, 160, 1);
            delay(500);
            return;
        }
        if (digitalRead(leleConfigPins.NEXT) == LOW) {
            selected = selected % 3 + 1;
            delay(200);
        }
        if (digitalRead(leleConfigPins.PREV) == LOW) {
            selected = selected == 1 ? 3 : selected - 1;
            delay(200);
        }
        delay(10);
    }
}

void applyPowerMode() {
    OptimizationMode mode;
    switch (currentPowerMode) {
        case ECONOMY:
            mode = MODE_POWERSAVE;
            break;
        case BALANCED:
            mode = MODE_BALANCED;
            break;
        case FORCE:
            mode = MODE_PERFORMANCE;
            break;
    }
    optimizationManager.setMode(mode);
}

/*********************************************************************
 **  Function: begin_storage
 **  Config LittleFS and SD storage
 *********************************************************************/
void begin_storage() {
    if (!LittleFS.begin(true)) { LittleFS.format(), LittleFS.begin(); }
    bool checkFS = setupSdCard();
    leleConfig.fromFile(checkFS);
    leleConfigPins.fromFile(checkFS);
}

/*********************************************************************
 **  Function: _setup_gpio()
 **  Sets up a weak (empty) function to be replaced by /ports/* /interface.h
 *********************************************************************/
void _setup_gpio() __attribute__((weak));
void _setup_gpio() {}

/*********************************************************************
 **  Function: _post_setup_gpio()
 **  Sets up a weak (empty) function to be replaced by /ports/* /interface.h
 *********************************************************************/
void _post_setup_gpio() __attribute__((weak));
void _post_setup_gpio() {}

/*********************************************************************
 **  Function: setup_gpio
 **  Setup GPIO pins
 *********************************************************************/
void setup_gpio() {

    // init setup from /ports/*/interface.h
    _setup_gpio();

    // Smoochiee v2 uses a AW9325 tro control GPS, MIC, Vibro and CC1101 RX/TX powerlines
    ioExpander.init(IO_EXPANDER_ADDRESS, &Wire);

#if TFT_MOSI > 0
    if (leleConfigPins.CC1101_bus.mosi == (gpio_num_t)TFT_MOSI)
        initCC1101once(&tft.getSPIinstance()); // (T_EMBED), CORE2 and others
    else
#endif
        if (leleConfigPins.CC1101_bus.mosi == leleConfigPins.SDCARD_bus.mosi)
        initCC1101once(&sdcardSPI); // (ARDUINO_M5STACK_CARDPUTER) and (ESP32S3DEVKITC1) and devices that
                                    // share CC1101 pin with only SDCard
    else initCC1101once(NULL);
    // (ARDUINO_M5STICK_C_PLUS) || (ARDUINO_M5STICK_C_PLUS2) and others that doesn´t share SPI with
    // other devices (need to change it when Lele board comes to shore)
}

/*********************************************************************
 **  Function: begin_tft
 **  Config tft
 *********************************************************************/
void begin_tft() {
    tft.setRotation(leleConfig.rotation); // sometimes it misses the first command
    tft.invertDisplay(leleConfig.colorInverted);
    tft.setRotation(leleConfig.rotation);
    tftWidth = tft.width();
#ifdef HAS_TOUCH
    tftHeight = tft.height() - 20;
#else
    tftHeight = tft.height();
#endif
    resetTftDisplay();
    setBrightness(leleConfig.bright, false);
}

/*********************************************************************
 **  Function: boot_screen
 **  Draw boot screen
 *********************************************************************/
void boot_screen() {
    tft.setTextColor(leleConfig.priColor, leleConfig.bgColor);
    tft.setTextSize(FM);
    tft.drawPixel(0, 0, leleConfig.bgColor);
    tft.drawCentreString("LELE", tftWidth / 2, 10, 1);
    tft.setTextSize(FP);
    tft.drawCentreString(LELE_VERSION, tftWidth / 2, 25, 1);
    tft.setTextSize(FM);
    tft.drawCentreString(
        "PREDATORY FIRMWARE", tftWidth / 2, tftHeight + 2, 1
    ); // will draw outside the screen on non touch devices
}

/*********************************************************************
 **  Function: boot_screen_anim
 **  Draw boot screen
 *********************************************************************/
void boot_screen_anim() {
    boot_screen();
    int i = millis();
    // checks for boot.jpg in SD and LittleFS for customization
    int boot_img = 0;
    bool drawn = false;
    if (sdcardMounted) {
        if (SD.exists("/boot.jpg")) boot_img = 1;
        else if (SD.exists("/boot.gif")) boot_img = 3;
    }
    if (boot_img == 0 && LittleFS.exists("/boot.jpg")) boot_img = 2;
    else if (boot_img == 0 && LittleFS.exists("/boot.gif")) boot_img = 4;
    if (leleConfig.theme.boot_img) boot_img = 5; // override others

    tft.drawPixel(0, 0, 0);       // Forces back communication with TFT, to avoid ghosting
                                  // Start image loop
    while (millis() < i + 7000) { // boot image lasts for 5 secs
        if ((millis() - i > 2000) && !drawn) {
            tft.fillRect(0, 45, tftWidth, tftHeight - 45, leleConfig.bgColor);
            if (boot_img > 0 && !drawn) {
                tft.fillScreen(leleConfig.bgColor);
                if (boot_img == 5) {
                    drawImg(
                        *leleConfig.themeFS(),
                        leleConfig.getThemeItemImg(leleConfig.theme.paths.boot_img),
                        0,
                        0,
                        true,
                        3600
                    );
                    Serial.println("Image from SD theme");
                } else if (boot_img == 1) {
                    drawImg(SD, "/boot.jpg", 0, 0, true);
                    Serial.println("Image from SD");
                } else if (boot_img == 2) {
                    drawImg(LittleFS, "/boot.jpg", 0, 0, true);
                    Serial.println("Image from LittleFS");
                } else if (boot_img == 3) {
                    drawImg(SD, "/boot.gif", 0, 0, true, 3600);
                    Serial.println("Image from SD");
                } else if (boot_img == 4) {
                    drawImg(LittleFS, "/boot.gif", 0, 0, true, 3600);
                    Serial.println("Image from LittleFS");
                }
                tft.drawPixel(0, 0, 0); // Forces back communication with TFT, to avoid ghosting
            }
            drawn = true;
        }
#if !defined(LITE_VERSION)
        if (!boot_img && (millis() - i > 2200) && (millis() - i) < 2700)
            tft.drawRect(2 * tftWidth / 3, tftHeight / 2, 2, 2, leleConfig.priColor);
        if (!boot_img && (millis() - i > 2700) && (millis() - i) < 2900)
            tft.fillRect(0, 45, tftWidth, tftHeight - 45, leleConfig.bgColor);
        if (!boot_img && (millis() - i > 2900) && (millis() - i) < 3400)
            tft.drawXBitmap(
                2 * tftWidth / 3 - 30,
                5 + tftHeight / 2,
                Lele_small_bits,
                Lele_small_width,
                Lele_small_height,
                leleConfig.bgColor,
                leleConfig.priColor
            );
        if (!boot_img && (millis() - i > 3400) && (millis() - i) < 3600) tft.fillScreen(leleConfig.bgColor);
        if (!boot_img && (millis() - i > 3600))
            tft.drawXBitmap(
                (tftWidth - 238) / 2,
                (tftHeight - 133) / 2,
                bits,
                bits_width,
                bits_height,
                leleConfig.bgColor,
                leleConfig.priColor
            );
#endif
        if (check(AnyKeyPress)) // If any key or M5 key is pressed, it'll jump the boot screen
        {
            tft.fillScreen(leleConfig.bgColor);
            delay(10);
            return;
        }
    }

    // Clear splashscreen
    tft.fillScreen(leleConfig.bgColor);
}

/*********************************************************************
 **  Function: init_clock
 **  Clock initialisation for propper display in menu
 *********************************************************************/
void init_clock() {
#if defined(HAS_RTC)

    _rtc.begin();
    _rtc.GetBm8563Time();
    _rtc.GetTime(&_time);
#endif
}

/*********************************************************************
 **  Function: init_led
 **  Led initialisation
 *********************************************************************/
void init_led() {
#ifdef HAS_RGB_LED
    beginLed();
#endif
}

/*********************************************************************
 **  Function: startup_sound
 **  Play sound or tone depending on device hardware
 *********************************************************************/
void startup_sound() {
    if (leleConfig.soundEnabled == 0) return; // if sound is disabled, do not play sound
#if !defined(LITE_VERSION)
#if defined(BUZZ_PIN)
    // Bip M5 just because it can. Does not bip if splashscreen is bypassed
    _tone(5000, 50);
    delay(200);
    _tone(5000, 50);
    /*  2fix: menu infinite loop */
#elif defined(HAS_NS4168_SPKR)
    // play a boot sound
    if (leleConfig.theme.boot_sound) {
        playAudioFile(leleConfig.themeFS(), leleConfig.getThemeItemImg(leleConfig.theme.paths.boot_sound));
    } else if (SD.exists("/boot.wav")) {
        playAudioFile(&SD, "/boot.wav");
    } else if (LittleFS.exists("/boot.wav")) {
        playAudioFile(&LittleFS, "/boot.wav");
    }
#endif
#endif
}

// #include "core/aggressive_sd.h" // Removed
#include "core/secure_boot.h"

// ESP32-S3 Optimization Modules
#ifdef CYD_S3
#include "core/thermal_manager.h"
#include "core/power_gating.h"
#include "core/gesture_wake.h"
#include "hardware/s3_hardware_init.h"
#include "core/s3_performance.h"
#include "core/audio_dma.h"
#include "core/paj7620u2.h"
#include "core/ai_attack_learner.h"
#endif

/*********************************************************************
 **  Function: setup
 **  Where the devices are started and variables set
 *********************************************************************/
void setup() {
    aggressive_boot_logic(); // Replaced by SecureBoot
    SecureBoot::run();

    Serial.setRxBufferSize(
        SAFE_STACK_BUFFER_SIZE / 4
    ); // Must be invoked before Serial.begin(). Default is 256 chars
    Serial.begin(115200);
    uart_set_baudrate(UART_NUM_1, 38400);

    log_d("Total heap: %d", ESP.getHeapSize());
    log_d("Free heap: %d", ESP.getFreeHeap());
    if (psramInit()) log_d("PSRAM Started");
    if (psramFound()) log_d("PSRAM Found");
    else log_d("PSRAM Not Found");
    log_d("Total PSRAM: %d", ESP.getPsramSize());
    log_d("Free PSRAM: %d", ESP.getFreePsram());

    // declare variables
    prog_handler = 0;
    sdcardMounted = false;
    wifiConnected = false;
    BLEConnected = false;
    leleConfig.bright = 100; // theres is no value yet
    leleConfig.rotation = ROTATION;
    setup_gpio();
#if defined(HAS_SCREEN)
    tft.init();
    tft.setRotation(leleConfig.rotation);
    tft.fillScreen(TFT_BLACK);
    // leleConfig is not read yet.. just to show something on screen due to long boot time
    tft.setTextColor(TFT_PURPLE, TFT_BLACK);
    tft.drawCentreString("Booting", tft.width() / 2, tft.height() / 2, 1);
#else
    tft.begin();
#endif
    begin_storage();
    preferences.begin("lele", false);
    currentPowerMode = (PowerMode) preferences.getUChar("bat_mode", BALANCED);
    begin_tft();
    showBootMenu();
    applyPowerMode();
    init_clock();
    init_led();

    // Some GPIO Settings (such as CYD's brightness control must be set after tft and sdcard)
    _post_setup_gpio();
    // end of post gpio begin

    // #ifndef USE_TFT_eSPI_TOUCH
    // This task keeps running all the time, will never stop
    xTaskCreate(
        taskInputHandler,              // Task function
        "InputHandler",                // Task Name
        INPUT_HANDLER_TASK_STACK_SIZE, // Stack size
        NULL,                          // Task parameters
        2,                             // Task priority (0 to 3), loopTask has priority 2.
        &xHandle                       // Task handle (not used)
    );
    // #endif
#if defined(HAS_SCREEN)
    leleConfig.openThemeFile(leleConfig.themeFS(), leleConfig.themePath, false);
    if (!leleConfig.instantBoot) {
        boot_screen_anim();
        boot_screen_hw(); // Hardware scan + SD Card check
        startup_sound();
    } else {
        // Verifica SD silenciosamente mesmo no instantBoot
        if (setupSdCard()) {
            sdCardReady = true;
            sd_create_structure();
        }
    }
    if (leleConfig.wifiAtStartup) {
        log_i("Loading Wifi at Startup");
        xTaskCreate(
            wifiConnectTask,   // Task function
            "wifiConnectTask", // Task Name
            4096,              // Stack size
            NULL,              // Task parameters
            2,                 // Task priority (0 to 3), loopTask has priority 2.
            NULL               // Task handle (not used)
        );
    }
#endif
    //  start a task to handle serial commands while the webui is running
    startSerialCommandsHandlerTask();

    wakeUpScreen();

    // Initialize Optimization Manager
    optimizationManager.begin();
    PanicMode::getInstance().init();

    // ESP32-S3 specific optimizations
#ifdef CYD_S3
    // Initialize ALL S3 hardware components
    if (!init_all_s3_hardware()) {
        log_w("[S3] Some hardware components failed to initialize");
    }
    
    power_gating_init();      // IRF520 MOSFET power control
    thermal_manager_init();   // DS18B20 thermal throttling (240->160MHz)
    s3_perf_init();           // Dynamic clock (240MHz) + battery throttle
    
    #ifdef HAS_GESTURE
    if (paj7620_init(&Wire)) {
        paj7620_enable_interrupt();  // Enable for deep sleep wake
        log_i("[S3] PAJ7620U2 gesture sensor initialized");
    }
    #endif
    
    log_i("[S3] Performance manager @ 240MHz with thermal/battery throttle");
    log_i("[S3] Power gating initialized for %d modules", MODULE_COUNT);
#endif

    // Initialize TTS system`n    init_tts_main();`n`n    // Initialize AI Attack Learner
    if (!aiLearner.init()) {
        log_w("[AI] Failed to initialize attack learner");
    } else {
        log_i("[AI] Attack learner initialized successfully");
    }

    // Initialize Persistence and Stealth systems
    init_persistence();
    check_stealth_wake();
    init_stealth();

    if (leleConfig.startupApp != "" && !startupApp.startApp(leleConfig.startupApp)) {
        leleConfig.setStartupApp("");
    }
}

/**********************************************************************
 **  Function: loop
 **  Main loop
 **********************************************************************/
#if defined(HAS_SCREEN)
void loop() {
    // Feed the paranoid watchdog
    SecureBoot::loopCheck();

    // Run optimization manager loop
    optimizationManager.loop();
    
    // Panic Mode Check
    PanicMode::getInstance().update();
    if (touchPoint.pressed) {
        PanicMode::getInstance().checkTrigger(touchPoint.x, touchPoint.y);
    }
    if (PanicMode::getInstance().isActive()) {
        lv_timer_handler(); // Ensure LVGL updates for Panic UI
        delay(5);
        return; // BLOCK MAIN MENU
    }

    // Update console interface if active
    updateConsoleInterface();

    // Update IoT interface if active
    updateIoTInterface();

    // Interpreter must be ran in the loop() function, otherwise it breaks
    // called by 'stack canary watchpoint triggered (loopTask)'
#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
    if (interpreter_start) {
        TaskHandle_t interpreterTaskHandler = NULL;
        vTaskDelete(serialcmdsTaskHandle); // stop serial commands while in interpreter
        vTaskDelay(pdMS_TO_TICKS(10));
        xTaskCreate(
            interpreterHandler,          // Task function
            "interpreterHandler",        // Task Name
            INTERPRETER_TASK_STACK_SIZE, // Stack size
            NULL,                        // Task parameters
            2,                           // Task priority (0 to 3), loopTask has priority 2.
            &interpreterTaskHandler      // Task handle
        );

        while (interpreter_start == true) { vTaskDelay(pdMS_TO_TICKS(500)); }
        startSerialCommandsHandlerTask();
        interpreter_start = false;
        previousMillis = millis(); // ensure that will not dim screen when get back to menu
    }
#endif
    tft.fillScreen(leleConfig.bgColor);

    // Run periodic stealth tasks
    stealth_periodic_tasks();

    mainMenu.begin();
    delay(1);
}
#else

void loop() {
    tft.setLogging();
    Serial.println(
        "\n"
        "¦¦¦¦¦¦  ¦¦¦¦¦¦  ¦¦    ¦¦  ¦¦¦¦¦¦ ¦¦¦¦¦¦¦ \n"
        "¦¦   ¦¦ ¦¦   ¦¦ ¦¦    ¦¦ ¦¦      ¦¦      \n"
        "¦¦¦¦¦¦  ¦¦¦¦¦¦  ¦¦    ¦¦ ¦¦      ¦¦¦¦¦   \n"
        "¦¦   ¦¦ ¦¦   ¦¦ ¦¦    ¦¦ ¦¦      ¦¦      \n"
        "¦¦¦¦¦¦  ¦¦   ¦¦  ¦¦¦¦¦¦   ¦¦¦¦¦¦ ¦¦¦¦¦¦¦ \n"
        "                                         \n"
        "         PREDATORY FIRMWARE\n\n"
        "Tips: Connect to the WebUI for better experience\n"
        "      Add your network by sending: wifi add ssid password\n\n"
        "At your command:"
    );

    // Enable navigation through webUI
    tft.fillScreen(leleConfig.bgColor);
    mainMenu.begin();
    vTaskDelay(10 / portTICK_PERIOD_MS);
}
#endif
`n// TTS and Deep Sleep variables`nbool speech_pending = false;`nunsigned long last_speech_time = 0;`nbool attack_running = false;`n`n// Initialize TTS system`nvoid init_tts_main() {`n    if (!init_tts_system()) {`n        log_w("[TTS] Failed to initialize TTS system");`n    } else {`n        log_i("[TTS] TTS system initialized successfully");`n        think_and_speak("Sistema pronto. Iniciando monitoramento.");`n    }`n}



