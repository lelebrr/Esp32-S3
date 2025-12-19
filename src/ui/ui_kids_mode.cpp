/**
 * @file ui_kids_mode.cpp
 * @brief Modo Kids - STUB (funcionalidade removida)
 * @note Esta funcionalidade foi desabilitada para economizar memória.
 */

#include "ui_kids_mode.h"

KidsMode kidsMode;

KidsMode::KidsMode() : _screen(nullptr), _dragonLabel(nullptr), _moodBar(nullptr),
    _hungerBar(nullptr), _feedBtn(nullptr), _playBtn(nullptr),
    _sleepBtn(nullptr), _statusLabel(nullptr), _dragonMood(0),
    _dragonHunger(0), _dragonEnergy(0), _lastUpdateTime(0) {
    _config.enabled = false;
}

void KidsMode::begin() {}
void KidsMode::enable() {}
bool KidsMode::disable(const char*) { return true; }
void KidsMode::createUI() {}
void KidsMode::update() {}
void KidsMode::updateStats() {}
void KidsMode::updateDragonFace() {}
void KidsMode::feedDragon() {}
void KidsMode::playWithDragon() {}
void KidsMode::sleepDragon() {}
void KidsMode::showHeart() {}
void KidsMode::showStar() {}
void KidsMode::setConfig(const KidsModeConfig&) {}
void KidsMode::setParentPin(const char*) {}
