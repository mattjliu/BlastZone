#include "raylib.h"
#include "Bomb.h"

Bomb::Bomb(float xPosition, float yPosition, float blastDuration, float countDownDuration)
  : xPosition(xPosition), yPosition(yPosition), blastDuration(blastDuration),
    countDownDuration(countDownDuration), blastZone(0.0001f, 1000)
{
  countDownElapsedTime = 0;
  blastStarted = false;
  blastOver = false;
  playerChecked = false;
  spriteTintRatio = 1.0f;
  blastSoundPlayed = false;
  lastPlayedBeepSoundTime = -1;
}

float Bomb::getXPosition() const
{
  return xPosition;
}

float Bomb::getYPosition() const
{
  return yPosition;
}

float Bomb::getBlastAlpha() const
{
  return blastAlpha;
}

float Bomb::getElapsedTime() const
{
  return blastElapsedTime;
}

Color Bomb::getSpriteTint() const
{
  return (Color) { 255, spriteTintRatio * 255, spriteTintRatio * 255, 255 };
}

bool Bomb::isBlastStarted() const
{
  return blastStarted;
}

bool Bomb::isBlastOver() const
{
  return blastOver;
}

bool Bomb::isPlayerInBlast(const Player& player) const
{
  return blastZone.isPlayerInBlastZone(xPosition, yPosition, player);
}

bool Bomb::update(float frameTime)
{
  if (!blastStarted)
  {
    return updateCountDown(frameTime);
  }
  else
  {
    updateBlast(frameTime);
    return false;
  }
}

bool Bomb::updateCountDown(float frameTime)
{
  countDownElapsedTime += frameTime;
  spriteTintRatio = 1 - (countDownElapsedTime - (int) countDownElapsedTime);
  if (countDownElapsedTime > countDownDuration)
  {
    startBlast(frameTime);
    blastStarted = true;
    updateBlast(countDownElapsedTime - countDownDuration);
    return true;
  }
  return false;
}

void Bomb::updateBlast(float frameTime)
{
  blastElapsedTime += frameTime;
  blastOver = blastElapsedTime > blastDuration ? true : false;
  blastAlpha = blastOver ? 0.0f : 1.0f - (blastElapsedTime / blastDuration);
}

void Bomb::startBlast(float frameTime)
{
  blastElapsedTime = 0;
  blastAlpha = 1.0f;
}

bool Bomb::draw(const std::vector<Edge>& edgeMap, const Player& player)
{
  if (!blastOver)
  {
    blastZone.drawBlastZone(xPosition, yPosition, edgeMap, blastAlpha);
    if (!playerChecked)
    {
      playerChecked = true;
      if (isPlayerInBlast(player))
      {
        return true;
      } 
    }
  }
  return false;
}

bool Bomb::isBlastSoundPlayed() const
{
  return blastSoundPlayed;
}

void Bomb::setBlastSoundPlayed()
{
  blastSoundPlayed = true;
}

bool Bomb::shouldPlayBeepSound() const
{
  if (!blastStarted)
    return lastPlayedBeepSoundTime != (int) countDownElapsedTime;
  else
    return false;
}

void Bomb::setLastPlayedBeepSoundTime()
{
  lastPlayedBeepSoundTime = (int) countDownElapsedTime;
}