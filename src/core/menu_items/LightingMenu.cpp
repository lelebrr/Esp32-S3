#include "LightingMenu.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/utils.h"
#include "modules/lighting/ws2812b_control.h"

// Variavel para LED selecionado (0 = todos, 1-4 = individual)
static uint8_t selectedLed = 0;

void LightingMenu::optionsMenu() {
    options = {
        {String(areLedsOn() ? "Desligar LEDs" : "Ligar LEDs"),
         [=]() {
             if (areLedsOn()) {
                 turnOffLeds();
                 displaySuccess("LEDs desligados", true);
             } else {
                 turnOnLeds();
                 displaySuccess("LEDs ligados", true);
             }
         }},
        {"Cor",           [this]() { colorMenu(); }},
        {"Selecionar LED", [this]() { selectLedMenu(); }},
        {"Efeitos",       [this]() { effectsMenu(); }},
        {"Brilho",        [this]() { brightnessMenu(); }},
        {"Notificacoes",  [this]() { notificationsMenu(); }},
        {"Testar LEDs",
         [=]() {
             displayInfo("Testando LEDs...", true);

             // Teste sequencial de cores
             setAllLeds(COLOR_RED);
             delay(300);
             setAllLeds(COLOR_GREEN);
             delay(300);
             setAllLeds(COLOR_BLUE);
             delay(300);
             setAllLeds(COLOR_WHITE);
             delay(300);
             setAllLeds(COLOR_OFF);

             displaySuccess("Teste concluido", true);
         }},
    };

    addOptionToMainMenu();
    loopOptions(options, MENU_TYPE_SUBMENU, "Iluminacao");
}

void LightingMenu::colorMenu() {
    options = {
        {"Vermelho", [=]() { if (selectedLed==0) setAllLeds(COLOR_RED); else setLedColor(selectedLed-1, COLOR_RED); }},
        {"Verde",    [=]() { if (selectedLed==0) setAllLeds(COLOR_GREEN); else setLedColor(selectedLed-1, COLOR_GREEN); }},
        {"Azul",     [=]() { if (selectedLed==0) setAllLeds(COLOR_BLUE); else setLedColor(selectedLed-1, COLOR_BLUE); }},
        {"Amarelo",  [=]() { if (selectedLed==0) setAllLeds(COLOR_YELLOW); else setLedColor(selectedLed-1, COLOR_YELLOW); }},
        {"Ciano",    [=]() { if (selectedLed==0) setAllLeds(COLOR_CYAN); else setLedColor(selectedLed-1, COLOR_CYAN); }},
        {"Magenta",  [=]() { if (selectedLed==0) setAllLeds(COLOR_MAGENTA); else setLedColor(selectedLed-1, COLOR_MAGENTA); }},
        {"Branco",   [=]() { if (selectedLed==0) setAllLeds(COLOR_WHITE); else setLedColor(selectedLed-1, COLOR_WHITE); }},
        {"Laranja",  [=]() { if (selectedLed==0) setAllLeds(COLOR_ORANGE); else setLedColor(selectedLed-1, COLOR_ORANGE); }},
        {"Roxo",     [=]() { if (selectedLed==0) setAllLeds(COLOR_PURPLE); else setLedColor(selectedLed-1, COLOR_PURPLE); }},
        {"Desligar", [=]() { if (selectedLed==0) setAllLeds(COLOR_OFF); else setLedColor(selectedLed-1, COLOR_OFF); }},
        {"Voltar",   [this]() { optionsMenu(); }},
    };

    const char* title = selectedLed == 0 ? "Cor - Todos" : "Cor - LED";
    loopOptions(options, MENU_TYPE_SUBMENU, title);
}

void LightingMenu::selectLedMenu() {
    options = {
        {String("Todos") + (selectedLed == 0 ? " *" : ""), [=]() { selectedLed = 0; displayInfo("Todos", true); }},
        {String("LED 1") + (selectedLed == 1 ? " *" : ""), [=]() { selectedLed = 1; displayInfo("LED 1", true); }},
        {String("LED 2") + (selectedLed == 2 ? " *" : ""), [=]() { selectedLed = 2; displayInfo("LED 2", true); }},
        {String("LED 3") + (selectedLed == 3 ? " *" : ""), [=]() { selectedLed = 3; displayInfo("LED 3", true); }},
        {String("LED 4") + (selectedLed == 4 ? " *" : ""), [=]() { selectedLed = 4; displayInfo("LED 4", true); }},
        {"Voltar", [this]() { optionsMenu(); }},
    };

    loopOptions(options, MENU_TYPE_SUBMENU, "Selecionar LED");
}

void LightingMenu::effectsMenu() {
    LightingEffect current = getCurrentEffect();

    // Menu Principal de Efeitos - Dividido em categorias
    options = {
        {"Efeitos Basicos", [this, current]() {
            options = {
                {String("Estatico") + (current == EFFECT_STATIC ? " *" : ""), [=]() { setEffect(EFFECT_STATIC); }},
                {String("Piscar") + (current == EFFECT_BLINK ? " *" : ""), [=]() { setEffect(EFFECT_BLINK); }},
                {String("Fade") + (current == EFFECT_FADE ? " *" : ""), [=]() { setEffect(EFFECT_FADE); }},
                {String("Arco-Iris") + (current == EFFECT_RAINBOW ? " *" : ""), [=]() { setEffect(EFFECT_RAINBOW); }},
                {String("Respirar") + (current == EFFECT_BREATHE ? " *" : ""), [=]() { setEffect(EFFECT_BREATHE); }},
                {String("Scanner") + (current == EFFECT_SCANNER ? " *" : ""), [=]() { setEffect(EFFECT_SCANNER); }},
                {String("Aleatorio") + (current == EFFECT_RANDOM ? " *" : ""), [=]() { setEffect(EFFECT_RANDOM); }},
                {String("Perseguir") + (current == EFFECT_CHASE ? " *" : ""), [=]() { setEffect(EFFECT_CHASE); }},
                {"Voltar", [this]() { effectsMenu(); }},
            };
            loopOptions(options, MENU_TYPE_SUBMENU, "Efeitos Basicos");
        }},
        {"Efeitos Avancados", [this, current]() {
            options = {
                {String("Pulso Indep.") + (current == EFFECT_INDEPENDENT_PULSE ? " *" : ""), [=]() { setEffect(EFFECT_INDEPENDENT_PULSE); }},
                {String("Onda de Cores") + (current == EFFECT_COLOR_WAVE ? " *" : ""), [=]() { setEffect(EFFECT_COLOR_WAVE); }},
                {String("Ping Pong") + (current == EFFECT_PING_PONG ? " *" : ""), [=]() { setEffect(EFFECT_PING_PONG); }},
                {String("Fogo") + (current == EFFECT_FIRE ? " *" : ""), [=]() { setEffect(EFFECT_FIRE); }},
                {String("Gelo") + (current == EFFECT_ICE ? " *" : ""), [=]() { setEffect(EFFECT_ICE); }},
                {String("Policia") + (current == EFFECT_POLICE ? " *" : ""), [=]() { setEffect(EFFECT_POLICE); }},
                {String("Arco-Iris Gira") + (current == EFFECT_RAINBOW_SPIN ? " *" : ""), [=]() { setEffect(EFFECT_RAINBOW_SPIN); }},
                {String("Batimento") + (current == EFFECT_HEARTBEAT ? " *" : ""), [=]() { setEffect(EFFECT_HEARTBEAT); }},
                {"Voltar", [this]() { effectsMenu(); }},
            };
            loopOptions(options, MENU_TYPE_SUBMENU, "Efeitos Avancados");
        }},
        {"Velocidade", [this]() {
            options = {
                {"Lento", [=]() { setEffectSpeed(2); displaySuccess("Vel: Lento", true); }},
                {"Normal", [=]() { setEffectSpeed(5); displaySuccess("Vel: Normal", true); }},
                {"Rapido", [=]() { setEffectSpeed(8); displaySuccess("Vel: Rapido", true); }},
                {"Turbo", [=]() { setEffectSpeed(10); displaySuccess("Vel: Turbo", true); }},
                {"Voltar", [this]() { effectsMenu(); }},
            };
            loopOptions(options, MENU_TYPE_SUBMENU, "Velocidade Efeito");
        }},
        {"Voltar", [this]() { optionsMenu(); }},
    };

    loopOptions(options, MENU_TYPE_SUBMENU, "Categorias Efeitos");
}

void LightingMenu::brightnessMenu() {
    uint8_t current = getLedBrightnessLevel();

    options = {
        {String("100%") + (current >= 250 ? " *" : ""), [=]() { setLedBrightnessLevel(255); displayInfo("Brilho: 100%", true); }},
        {String("75%") + (current >= 180 && current < 250 ? " *" : ""), [=]() { setLedBrightnessLevel(191); displayInfo("Brilho: 75%", true); }},
        {String("50%") + (current >= 120 && current < 180 ? " *" : ""), [=]() { setLedBrightnessLevel(127); displayInfo("Brilho: 50%", true); }},
        {String("25%") + (current >= 50 && current < 120 ? " *" : ""), [=]() { setLedBrightnessLevel(64); displayInfo("Brilho: 25%", true); }},
        {String("10%") + (current < 50 ? " *" : ""), [=]() { setLedBrightnessLevel(25); displayInfo("Brilho: 10%", true); }},
        {"Voltar", [this]() { optionsMenu(); }},
    };

    loopOptions(options, MENU_TYPE_SUBMENU, "Brilho LED");
}

void LightingMenu::notificationsMenu() {
    options = {
        {"LED p/ Erros", [=]() { notifyError(); }},
        {"LED p/ Sucesso", [=]() { notifySuccess(); }},
        {"LED p/ WiFi", [=]() { notifyWiFi(true); }},
        {"LED p/ Captura", [=]() { notifyCapture(); }},
        {"Voltar", [this]() { optionsMenu(); }},
    };

    loopOptions(options, MENU_TYPE_SUBMENU, "Notificacoes LED");
}

void LightingMenu::drawIconImg() {
    drawImg(
        *leleConfig.themeFS(),
        leleConfig.getThemeItemImg(leleConfig.theme.paths.others),
        0,
        imgCenterY,
        true
    );
}

void LightingMenu::drawIcon(float scale) {
    clearIconArea();
    int radius = scale * 6;
    int spacing = radius * 2.5;
    int startX = iconCenterX - spacing * 1.5;

    tft.fillCircle(startX, iconCenterY, radius, TFT_RED);
    tft.fillCircle(startX + spacing, iconCenterY, radius, TFT_GREEN);
    tft.fillCircle(startX + spacing * 2, iconCenterY, radius, TFT_BLUE);
    tft.fillCircle(startX + spacing * 3, iconCenterY, radius, TFT_WHITE);

    tft.drawCircle(startX, iconCenterY, radius + 1, leleConfig.priColor);
    tft.drawCircle(startX + spacing, iconCenterY, radius + 1, leleConfig.priColor);
    tft.drawCircle(startX + spacing * 2, iconCenterY, radius + 1, leleConfig.priColor);
    tft.drawCircle(startX + spacing * 3, iconCenterY, radius + 1, leleConfig.priColor);
}
