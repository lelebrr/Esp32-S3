#pragma once

/**
 * @file ui_avatar.h
 * @brief Avatar visual do assistente de voz
 */

#include <Arduino.h>
#include <lvgl.h>

/**
 * @brief Estados do avatar
 */
enum AvatarState {
  AVATAR_IDLE,      // Normal
  AVATAR_LISTENING, // Ouvindo (aura pulsante)
  AVATAR_SPEAKING,  // Falando (boca animada)
  AVATAR_THINKING,  // Processando (loading spinner)
  AVATAR_HAPPY,     // Feliz
  AVATAR_SLEEPING   // Dormindo
};

/**
 * @brief Tipos de face simples
 */
enum FaceType {
  FACE_HAPPY = 0,
  FACE_SAD,
  FACE_EXCITED,
  FACE_ANGRY,
  FACE_SLEEP
};

/**
 * @brief Componente visual do assistente de voz
 */
class VoiceAvatar {
public:
  VoiceAvatar();

  void create(lv_obj_t *parent);
  void update();
  void setState(AvatarState state);
  AvatarState getState() const { return _state; }

  void setFace(FaceType face);
  void setText(const char *text);
  void setSubtext(const char *text);
  void setGlowColor(uint32_t color);
  void setGlowEnabled(bool enable);
  void setName(const char *name);

  void playWaveAnimation();
  void playSuccessAnimation();
  void playErrorAnimation();

  void show();
  void hide();

  void setFocus(int x, int y);
  void reactToTouch();
  void checkNoseTap(int x, int y);

private:
  lv_obj_t *_container;
  lv_obj_t *_avatarCircle;
  lv_obj_t *_faceLabel;
  lv_obj_t *_nameLabel;
  lv_obj_t *_textLabel;
  lv_obj_t *_subtextLabel;
  lv_obj_t *_auraRing;
  lv_obj_t *_listeningDots[3];

  AvatarState _state;
  FaceType _currentFace;
  uint32_t _glowColor;
  bool _glowEnabled;

  int _frameCount;
  int _blinkTimer;
  int _dotAnimFrame;
  int _noseTapCount;
  uint32_t _lastTapTime;

  void updateIdleAnimation();
  void updateListeningAnimation();
  void updateSpeakingAnimation();
  void updateThinkingAnimation();
  void createAuraEffect();
};

extern VoiceAvatar voiceAvatar;
