#pragma once
#include "raylib.h"
#include "Direction.h"
#include "Edge.h"
#include <array>
#include <string>

class Level;

class Player
{
public:
  Player(float, float, float);
  float getPositionX() const;
  float getPositionY() const;
  float getWidth() const;
  void move(Direction, float, const Level&);
  void draw() const;
  void drawLoss(float) const;
  void resetPlayer(float, float);
  std::array<Edge,4> getEdges() const;
private:
  const std::string PLAYER_SPRITE_PATH = "res/player.png";
  const int PLAYER_SPRITE_WIDTH = 20.f;
  Color color;
  Texture2D playerSprite;
  Direction spriteDirection;
  float velocity;
  float xPosition;
  float yPosition;
  float xPositionNew;
  float yPositionNew;
  bool checkCollision(Direction, float, float, const Level&) const;
  void resolveCollision(Direction, float, float, const Level&);
  Edge calculateEdge(Direction, float, float, float directionOffset=1.0f) const;
};