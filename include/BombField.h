#pragma once
#include "raylib.h"
#include "Bomb.h"
#include "Player.h"
#include <vector>
#include <string>

class BombField
{
public:
  BombField();
  ~BombField();
  void addBomb(Bomb*);
  bool update(float);
  bool drawBombs(const std::vector<Edge>&, const Player&);
  void clearBombField();
private:
  const std::string BOMB_SPRITE_PATH = "res/bomb.png";
  const int BOMB_SPRITE_WIDTH = 16;
  const std::string EXPLOSION_SOUND_PATH = "res/explosion.wav";
  const std::string BEEP_SOUND_PATH = "res/beep.wav";
  std::vector<Bomb*> bombs;
  Texture2D bombSprite;
  Sound explosionSound;
  Sound beepSound;
};