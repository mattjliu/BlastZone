#include "raylib.h"
#include "BombField.h"
#include "Bomb.h"
#include "Player.h"
#include <vector>
#include <string>

BombField::BombField()
{
  bombSprite = LoadTexture(BOMB_SPRITE_PATH.c_str());
  explosionSound = LoadSound(EXPLOSION_SOUND_PATH.c_str());
  beepSound = LoadSound(BEEP_SOUND_PATH.c_str());
}

BombField::~BombField()
{
  UnloadTexture(bombSprite);
  UnloadSound(explosionSound);
  UnloadSound(beepSound);
}

void BombField::addBomb(Bomb* bomb)
{
  bombs.push_back(bomb);
}

bool BombField::update(float frameTime)
{
  bool bombDetonated = false;
  for (auto it = bombs.begin(); it != bombs.end(); it++)
  {
    if ((*it)->update(frameTime))
      bombDetonated = true;
    if ((*it)->isBlastOver())
    {
      delete *it;
      bombs.erase(it--);
    }
  }
  return bombDetonated;
}

bool BombField::drawBombs(const std::vector<Edge>& edgeMap, const Player& player)
{
  bool playerHit = false;
  for (auto it = bombs.begin(); it != bombs.end(); it++)
  {
    if (!(*it)->isBlastStarted())
    {
      DrawTexture(bombSprite, (*it)->getXPosition() - BOMB_SPRITE_WIDTH / 2, (*it)->getYPosition() - BOMB_SPRITE_WIDTH / 2, (*it)->getSpriteTint());
      if ((*it)->shouldPlayBeepSound())
      {
        PlaySound(beepSound);
        (*it)->setLastPlayedBeepSoundTime();
      }
    }
    else
    {
      if ((*it)->draw(edgeMap, player))
        playerHit = true;
      if (!(*it)->isBlastSoundPlayed())
      {
        PlaySound(explosionSound);
        (*it)->setBlastSoundPlayed();
      }
    }
  }
  return playerHit;
}

void BombField::clearBombField()
{
  for (Bomb* bomb: bombs)
    delete bomb;
  bombs.clear();
}