#pragma once
#include "raylib.h"
#include "Edge.h"
#include "BlastZone.h"
#include "Player.h"

class Bomb
{
public:
  Bomb(float, float, float, float);
  float getXPosition() const;
  float getYPosition() const;
  float getBlastAlpha() const;
  float getElapsedTime() const;
  Color getSpriteTint() const;
  bool isBlastStarted() const;
  bool isBlastOver() const;
  bool isPlayerInBlast(const Player&) const;
  bool update(float);
  bool draw(const std::vector<Edge>&, const Player&);
  bool isBlastSoundPlayed() const;
  void setBlastSoundPlayed();
  bool shouldPlayBeepSound() const;
  void setLastPlayedBeepSoundTime();
private:
  float xPosition;
  float yPosition;
  float blastAlpha;
  float spriteTintRatio;
  float blastDuration;
  float blastElapsedTime;
  float countDownDuration;
  float countDownElapsedTime;
  bool blastStarted;
  bool blastOver;
  bool playerChecked;
  bool blastSoundPlayed;
  int lastPlayedBeepSoundTime;

  BlastZone blastZone;

  void startBlast(float);
  void updateBlast(float);
  bool updateCountDown(float);
};