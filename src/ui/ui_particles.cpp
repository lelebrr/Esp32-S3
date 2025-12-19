/**
 * @file ui_particles.cpp
 * @brief Implementação do sistema de partículas
 */

#include "ui_particles.h"

ParticleSystem particles;

// Paleta de cores cyberpunk padrão
static const uint32_t DEFAULT_COLORS[] = {
    0x00FF88, // Neon green
    0x00CCFF, // Cyan
    0xFF00FF, // Magenta
    0x9933FF, // Purple
    0x00FFFF, // Bright cyan
    0x33FF33  // Lime
};

ParticleSystem::ParticleSystem()
    : _container(nullptr), _effect(PARTICLE_FLOAT), _continuous(false),
      _emitRate(2), _frameCount(0), _colorCount(6), _screenWidth(368),
      _screenHeight(448) {

  memcpy(_colors, DEFAULT_COLORS, sizeof(DEFAULT_COLORS));

  for (int i = 0; i < MAX_PARTICLES; i++) {
    _particles[i].active = false;
    _particleObjs[i] = nullptr;
  }
}

void ParticleSystem::init(lv_obj_t *parent) {
  // Container transparente para partículas
  _container = lv_obj_create(parent);
  lv_obj_set_size(_container, lv_pct(100), lv_pct(100));
  lv_obj_align(_container, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_bg_opa(_container, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(_container, 0, 0);
  lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(_container, LV_OBJ_FLAG_CLICKABLE);

  // Pré-cria objetos de partícula (círculos pequenos)
  for (int i = 0; i < MAX_PARTICLES; i++) {
    _particleObjs[i] = lv_obj_create(_container);
    lv_obj_set_size(_particleObjs[i], 4, 4);
    lv_obj_set_style_radius(_particleObjs[i], LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(_particleObjs[i], 0, 0);
    lv_obj_add_flag(_particleObjs[i], LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(_particleObjs[i], LV_OBJ_FLAG_CLICKABLE);
  }
}

void ParticleSystem::update() {
  if (!_container)
    return;

  _frameCount++;

  // Emissão contínua
  if (_continuous && (_frameCount % (60 / _emitRate) == 0)) {
    emit(1);
  }

  // Atualiza cada partícula
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (_particles[i].active) {
      updateParticle(i);
    }
  }
}

void ParticleSystem::updateParticle(int index) {
  Particle &p = _particles[index];
  lv_obj_t *obj = _particleObjs[index];

  // Reduz vida
  if (p.life > 0) {
    p.life--;
  } else {
    p.active = false;
    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    return;
  }

  // Movimento baseado no efeito
  switch (_effect) {
  case PARTICLE_FLOAT:
    p.x += p.vx;
    p.y += p.vy + sin(_frameCount * 0.1 + index) * 0.5;
    p.vy -= 0.02; // Sobe suavemente
    break;

  case PARTICLE_RISE:
    p.x += p.vx + sin(_frameCount * 0.05 + index) * 0.3;
    p.y -= 1.5; // Sobe constante
    break;

  case PARTICLE_FALL:
    p.x += sin(_frameCount * 0.02 + index) * 0.2;
    p.y += p.vy;
    p.vy += 0.1; // Gravidade
    break;

  case PARTICLE_EXPLODE:
    p.x += p.vx;
    p.y += p.vy;
    p.vx *= 0.98; // Fricção
    p.vy *= 0.98;
    break;

  case PARTICLE_ORBIT: {
    float angle = _frameCount * 0.02 + index * 0.5;
    float radius = 50 + index * 2;
    p.x = _screenWidth / 2 + cos(angle) * radius;
    p.y = _screenHeight / 3 + sin(angle) * radius * 0.5;
    break;
  }

  case PARTICLE_SPARKLE:
    // Posição estática, apenas pisca
    p.alpha = (sin(_frameCount * 0.2 + index) + 1) * 127;
    break;
  }

  // Fade out baseado na vida restante
  float lifeRatio = (float)p.life / p.maxLife;
  p.alpha = (uint8_t)(lifeRatio * 255);

  // Limites da tela
  if (p.x < 0)
    p.x = _screenWidth;
  if (p.x > _screenWidth)
    p.x = 0;
  if (p.y < -20)
    p.active = false;
  if (p.y > _screenHeight + 20)
    p.active = false;

  // Atualiza objeto LVGL
  lv_obj_set_pos(obj, (int)p.x, (int)p.y);
  lv_obj_set_size(obj, (int)p.size, (int)p.size);
  lv_obj_set_style_bg_color(obj, lv_color_hex(p.color), 0);
  lv_obj_set_style_bg_opa(obj, p.alpha, 0);
}

void ParticleSystem::emit(int count, int x, int y) {
  if (x < 0)
    x = _screenWidth / 2;
  if (y < 0)
    y = _screenHeight / 2;

  int spawned = 0;
  for (int i = 0; i < MAX_PARTICLES && spawned < count; i++) {
    if (!_particles[i].active) {
      spawnParticle(i, x, y);
      spawned++;
    }
  }
}

void ParticleSystem::spawnParticle(int index, int x, int y) {
  Particle &p = _particles[index];
  lv_obj_t *obj = _particleObjs[index];

  p.active = true;
  p.x = x + random(-30, 30);
  p.y = y + random(-20, 20);
  p.size = random(PARTICLE_MIN_SIZE, PARTICLE_MAX_SIZE);
  p.color = getRandomColor();
  p.alpha = 255;
  p.maxLife = random(60, 180); // 1-3 segundos a 60fps
  p.life = p.maxLife;

  // Velocidade baseada no efeito
  switch (_effect) {
  case PARTICLE_FLOAT:
    p.vx = random(-10, 10) / 10.0;
    p.vy = random(-20, -5) / 10.0;
    break;
  case PARTICLE_RISE:
    p.vx = random(-5, 5) / 10.0;
    p.vy = -1.5;
    break;
  case PARTICLE_FALL:
    p.vx = random(-3, 3) / 10.0;
    p.vy = random(2, 8) / 10.0;
    break;
  case PARTICLE_EXPLODE:
    p.vx = random(-40, 40) / 10.0;
    p.vy = random(-40, 40) / 10.0;
    break;
  case PARTICLE_ORBIT:
  case PARTICLE_SPARKLE:
    p.vx = 0;
    p.vy = 0;
    break;
  }

  lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

void ParticleSystem::setEffect(ParticleEffect effect) { _effect = effect; }

void ParticleSystem::setContinuous(bool enable, int rate) {
  _continuous = enable;
  _emitRate = max(1, min(30, rate));
}

void ParticleSystem::setColors(const uint32_t *colors, int count) {
  _colorCount = min(8, count);
  memcpy(_colors, colors, sizeof(uint32_t) * _colorCount);
}

void ParticleSystem::clear() {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    _particles[i].active = false;
    if (_particleObjs[i]) {
      lv_obj_add_flag(_particleObjs[i], LV_OBJ_FLAG_HIDDEN);
    }
  }
}

void ParticleSystem::show() {
  if (_container)
    lv_obj_clear_flag(_container, LV_OBJ_FLAG_HIDDEN);
}

void ParticleSystem::hide() {
  if (_container)
    lv_obj_add_flag(_container, LV_OBJ_FLAG_HIDDEN);
}

uint32_t ParticleSystem::getRandomColor() {
  return _colors[random(0, _colorCount)];
}
