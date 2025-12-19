#include "AIMenu.h"
#include <globals.h>
#include "display.h"
#include "mykeyboard.h"
#include "sd_functions.h"
#include <FastLED.h>
#include <Wire.h>

AIMenu aiMenu;

AIMenu::AIMenu() {}

void AIMenu::optionsMenu() {
    options.clear();
    options.push_back({"Atualizar CVEs", [=]() { updateAttacks(); }, true});
    options.push_back({"Brute Force Brasil", [=]() { growBruteForceList(); }, true});
    options.push_back({"Feedback Certo", [=]() { qLearnFeedback(true); }, true});
    options.push_back({"Feedback Errado", [=]() { qLearnFeedback(false); }, true});
    options.push_back({"Gerar Novo Ataque", [=]() { generateNewAttack(); }, true});
    options.push_back({"Estatisticas IA", [=]() { showStats(); }, true});
    options.push_back({"Modo Stealth", [=]() { toggleStealth(); }, true});
    addOptionToMainMenu();
    loopOptions(options);
}

void AIMenu::draw() {
    tft.fillScreen(leleConfig.bgColor);
    tft.setTextColor(leleConfig.priColor, leleConfig.bgColor);
    tft.setTextSize(FM);
    tft.drawCentreString("IA Aprendiz", tftWidth / 2, 10, FM);

    tft.setTextSize(FP);
    tft.drawString("Q-Learning + TensorFlow", 10, 40);
    tft.drawString("Aprende com contexto", 10, 60);
    tft.drawString("GPS, Temp, Bateria", 10, 80);
    tft.drawString("Feedback manual", 10, 100);
    tft.drawString("Gera ataques novos", 10, 120);

    if (aiLearner.isStealthModeRequired()) {
        tft.setTextColor(TFT_RED, leleConfig.bgColor);
        tft.drawString("MODO STEALTH ATIVO", 10, 150);
    }
}

String AIMenu::getName() {
    return "IA Aprendiz";
}

void AIMenu::updateAttacks() {
    tft.fillScreen(leleConfig.bgColor);
    tft.setTextColor(leleConfig.priColor, leleConfig.bgColor);
    tft.drawCentreString("Atualizando CVEs...", tftWidth / 2, tftHeight / 2, FM);

    bool success = aiLearner.updateAttacksFromAPI();

    tft.fillScreen(leleConfig.bgColor);
    if (success) {
        tft.setTextColor(TFT_GREEN, leleConfig.bgColor);
        tft.drawCentreString("CVEs Atualizadas!", tftWidth / 2, tftHeight / 2, FM);
        FastLED.showColor(CRGB::Green);
        _tone(1000, 100);
    } else {
        tft.setTextColor(TFT_RED, leleConfig.bgColor);
        tft.drawCentreString("Falha na Atualizacao", tftWidth / 2, tftHeight / 2, FM);
        FastLED.showColor(CRGB::Red);
        _tone(500, 200);
    }
    delay(2000);
}

void AIMenu::qLearnFeedback(bool success) {
    tft.fillScreen(leleConfig.bgColor);
    tft.setTextColor(leleConfig.priColor, leleConfig.bgColor);
    tft.drawCentreString(success ? "Certo! +1" : "Errado! -1", tftWidth / 2, tftHeight / 2, FM);

    aiLearner.provideFeedback(success);

    if (success) {
        FastLED.showColor(CRGB::Green);
        _tone(1500, 100);
    } else {
        FastLED.showColor(CRGB::Red);
        _tone(500, 200);
    }
    delay(1500);
}

void AIMenu::generateNewAttack() {
    tft.fillScreen(leleConfig.bgColor);
    tft.setTextColor(leleConfig.priColor, leleConfig.bgColor);
    tft.drawCentreString("Gerando Ataque Novo...", tftWidth / 2, tftHeight / 2, FM);

    // Usar Q-table para criar variante baseada em sucesso passado
    ContextFeatures ctx = aiLearner.gatherContext();
    AttackType bestAttack;
    #ifdef HAS_TFLITE
        bestAttack = aiLearner.predictAttack(ctx);
    #else
        // Fallback or random attack when AI is not available
        bestAttack = (AttackType)(random(0, 8));
    #endif

    // Criar variante baseada no ataque de melhor sucesso
    String baseAttack;
    String variant;

    switch (bestAttack) {
        case ATTACK_DEAUTH:
            baseAttack = "deauth";
            variant = "deauth_delay_" + String(random(100, 500)) + "ms_boost_" + String(random(1, 5));
            break;
        case ATTACK_FAULT_INJECT:
            baseAttack = "fault_inject";
            variant = "nfc_fault_boost_" + String(random(3, 10)) + "V_delay_" + String(random(50, 200)) + "us";
            break;
        case ATTACK_REPLAY:
            baseAttack = "replay";
            variant = "subghz_replay_freq_" + String(433.92 + random(-10, 10)/10.0) + "MHz_var_" + String(random(5, 20)) + "ms";
            break;
        case ATTACK_EVIL_TWIN:
            baseAttack = "evil_twin";
            variant = "wifi_evil_twin_portal_" + String(random(1000, 9999)) + "_delay_" + String(random(2000, 5000)) + "ms";
            break;
        case ATTACK_NFC_SPOOF:
            baseAttack = "nfc_spoof";
            variant = "nfc_spoof_multi_" + String(random(2, 5)) + "_tags_boost_" + String(random(2, 8)) + "V";
            break;
        case ATTACK_IR_CLONE:
            baseAttack = "ir_clone";
            variant = "ir_clone_multi_zone_" + String(random(2, 5)) + "_repeat_" + String(random(3, 10));
            break;
        case ATTACK_CC1101_REPLAY:
            baseAttack = "cc1101_replay";
            variant = "cc1101_replay_mod_" + String(random(0, 3)) + "_freq_" + String(433.92 + random(-5, 5)/10.0) + "MHz";
            break;
        case ATTACK_NRF24_SNIFF:
            baseAttack = "nrf24_sniff";
            variant = "nrf24_sniff_channels_" + String(random(10, 50)) + "_boost_" + String(random(5, 20)) + "dBm";
            break;
        default:
            variant = "unknown_variant_" + String(random(10000, 99999));
            break;
    }

    // Salvar no SD como arquivo .cpp para OTA
    if (!SD.exists("/ai/payloads")) {
        SD.mkdir("/ai/payloads");
    }

    String filename = "/ai/payloads/" + variant + ".cpp";
    File attackFile = SD.open(filename, FILE_WRITE);
    if (attackFile) {
        attackFile.println("// Generated attack variant: " + variant);
        attackFile.println("// Base: " + baseAttack);
        attackFile.println("// Context: GPS=" + String(ctx.gps_lat, 2) + "," + String(ctx.gps_lon, 2));
        attackFile.println("// Temp=" + String(ctx.temperature) + "C, Batt=" + String(ctx.battery_voltage) + "V");
        attackFile.println("// Generated: " + String(millis()));
        attackFile.println();
        attackFile.println("// TODO: Implement actual attack code here");
        attackFile.println("void " + variant + "() {");
        attackFile.println("    // Attack implementation");
        attackFile.println("}");
        attackFile.close();

        tft.fillScreen(leleConfig.bgColor);
        tft.drawCentreString("Ataque Gerado:", tftWidth / 2, tftHeight / 2 - 30, FM);
        tft.drawCentreString(variant, tftWidth / 2, tftHeight / 2, FP);
        tft.drawCentreString("Salvo: " + filename, tftWidth / 2, tftHeight / 2 + 30, FP);

        FastLED.showColor(CRGB::Blue);
        _tone(2000, 100);
        delay(3000);
    } else {
        tft.fillScreen(leleConfig.bgColor);
        tft.setTextColor(TFT_RED, leleConfig.bgColor);
        tft.drawCentreString("Erro ao salvar!", tftWidth / 2, tftHeight / 2, FM);
        FastLED.showColor(CRGB::Red);
        _tone(500, 200);
        delay(2000);
    }
}

void AIMenu::showStats() {
    tft.fillScreen(leleConfig.bgColor);
    tft.setTextColor(leleConfig.priColor, leleConfig.bgColor);
    tft.setTextSize(FM);
    tft.drawCentreString("Estatisticas IA", tftWidth / 2, 10, FM);

    tft.setTextSize(FP);
    tft.drawString("Estado Atual: " + String(aiLearner.gatherContext().temperature) + "C", 10, 40);
    tft.drawString("Bateria: " + String(aiLearner.gatherContext().battery_voltage) + "V", 10, 60);
    tft.drawString("GPS: " + String(aiLearner.gatherContext().gps_lat, 2), 10, 80);
    tft.drawString("Stealth: " + String(aiLearner.isStealthModeRequired() ? "Sim" : "Nao"), 10, 100);

    tft.drawString("Pressione qualquer tecla", 10, tftHeight - 30);
    while (!check(AnyKeyPress)) delay(10);
}

void AIMenu::toggleStealth() {
    if (aiLearner.isStealthModeRequired()) {
        aiLearner.exitStealthMode();
        tft.fillScreen(leleConfig.bgColor);
        tft.setTextColor(TFT_GREEN, leleConfig.bgColor);
        tft.drawCentreString("Stealth DESATIVADO", tftWidth / 2, tftHeight / 2, FM);
    } else {
        aiLearner.enterStealthMode();
        tft.fillScreen(leleConfig.bgColor);
        tft.setTextColor(TFT_RED, leleConfig.bgColor);
        tft.drawCentreString("Stealth ATIVADO", tftWidth / 2, tftHeight / 2, FM);
    }
    delay(2000);
}

void AIMenu::growBruteForceList() {
    tft.fillScreen(leleConfig.bgColor);
    tft.setTextColor(leleConfig.priColor, leleConfig.bgColor);
    tft.drawCentreString("Brute Force Brasil", tftWidth / 2, tftHeight / 2 - 20, FM);
    tft.drawCentreString("Em breve...", tftWidth / 2, tftHeight / 2 + 20, FP);
    delay(2000);    
}
