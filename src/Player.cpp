#include "raylib.h"
#include "Player.h"
#include "Direction.h"
#include "Level.h"
#include "Edge.h"
#include <array>
#include <string>

Player::Player(float velocity, float xPosition, float yPosition)
  : color(BLACK), velocity(velocity), xPosition(xPosition), yPosition(yPosition)
{
  playerSprite = LoadTexture(PLAYER_SPRITE_PATH.c_str());
  spriteDirection = Direction::WEST;
}

float Player::getPositionX() const
{
  return xPosition;
}

float Player::getPositionY() const
{
  return yPosition;
}

float Player::getWidth() const
{
  return PLAYER_SPRITE_WIDTH;
}

void Player::move(Direction direction, float frameTime, const Level& level)
{
  switch (direction)
  {
    case Direction::NORTH:
    {
      yPositionNew = yPosition - frameTime * velocity;
      if (checkCollision(direction, xPosition, yPositionNew, level))
      {
        resolveCollision(direction, xPosition, yPositionNew, level);
      }
      yPosition = yPositionNew;
      break;
    }
    case Direction::SOUTH:
    {
      yPositionNew = yPosition + frameTime * velocity;
      if (checkCollision(direction, xPosition, yPositionNew, level))
      {
        resolveCollision(direction, xPosition, yPositionNew, level);
      }
      yPosition = yPositionNew;
      break;
    }
    case Direction::EAST:
    {
      xPositionNew = xPosition + frameTime * velocity;
      if (checkCollision(direction, xPositionNew, yPosition, level))
      {
        resolveCollision(direction, xPositionNew, yPosition, level);
      }
      xPosition = xPositionNew;
      spriteDirection = Direction::EAST;
      break;
    }
    case Direction::WEST:
    {
      xPositionNew = xPosition - frameTime * velocity;
      if (checkCollision(direction, xPositionNew, yPosition, level))
      {
        resolveCollision(direction, xPositionNew, yPosition, level);
      }
      xPosition = xPositionNew;
      spriteDirection = Direction::WEST;
      break;
    }
  }
}

void Player::draw() const
{
  if (spriteDirection == Direction::WEST)
    DrawTextureRec(playerSprite, { 0, 0, static_cast<float>(PLAYER_SPRITE_WIDTH), static_cast<float>(PLAYER_SPRITE_WIDTH) }, { xPosition, yPosition }, RAYWHITE);
  else
    DrawTextureRec(playerSprite, { static_cast<float>(PLAYER_SPRITE_WIDTH), 0, static_cast<float>(PLAYER_SPRITE_WIDTH), static_cast<float>(PLAYER_SPRITE_WIDTH) }, { xPosition, yPosition }, RAYWHITE);
}

void Player::drawLoss(float alpha) const
{
  if (spriteDirection == Direction::WEST)
    DrawTextureRec(playerSprite, { 0, 0, static_cast<float>(PLAYER_SPRITE_WIDTH), static_cast<float>(PLAYER_SPRITE_WIDTH) }, { xPosition, yPosition }, Fade(RAYWHITE, alpha));
  else
    DrawTextureRec(playerSprite, { static_cast<float>(PLAYER_SPRITE_WIDTH), 0, static_cast<float>(PLAYER_SPRITE_WIDTH), static_cast<float>(PLAYER_SPRITE_WIDTH) }, { xPosition, yPosition }, Fade(RAYWHITE, alpha));
}

void Player::resetPlayer(float xPosition, float yPosition)
{
  this->xPosition = xPosition;
  this->yPosition = yPosition;
  xPositionNew = xPosition;
  yPositionNew = yPosition;
}

std::array<Edge,4> Player::getEdges() const
{
  std::array<Edge,4> playerEdges;
  for (Direction direction : ALL_DIRECTIONS)
    playerEdges[static_cast<int>(direction)] = calculateEdge(direction, xPosition, yPosition);
  return playerEdges;
}

bool Player::checkCollision(Direction direction, float xPosition, float yPosition, const Level& level) const
{
  Edge collisionEdge = calculateEdge(direction, xPosition, yPosition, 0.9f);
  int startX = static_cast<int>(collisionEdge.startX);
  int startY = static_cast<int>(collisionEdge.startY);
  int endX = static_cast<int>(collisionEdge.endX);
  int endY = static_cast<int>(collisionEdge.endY);
  return (level.cellExistsAtCoordinate(startX, startY) || level.cellExistsAtCoordinate(endX, endY));
}

void Player::resolveCollision(Direction direction, float topLeftX, float topLeftY, const Level& level)
{
  int topLeftX_i = static_cast<int>(topLeftX);
  int topLeftY_i = static_cast<int>(topLeftY);
  switch (direction)
  {
    case Direction::NORTH:
      yPositionNew = static_cast<float>(level.getCellY(topLeftX_i, topLeftY_i) + level.getTileSize());
      break;
    case Direction::SOUTH:
      yPositionNew = static_cast<float>(level.getCellY(topLeftX_i, topLeftY_i + PLAYER_SPRITE_WIDTH) - PLAYER_SPRITE_WIDTH);
      break;
    case Direction::EAST:
      xPositionNew = static_cast<float>(level.getCellX(topLeftX_i + PLAYER_SPRITE_WIDTH, topLeftY_i) - PLAYER_SPRITE_WIDTH);
      break;
    case Direction::WEST:
      xPositionNew = static_cast<float>(level.getCellX(topLeftX_i, topLeftY_i) + level.getTileSize());
      break;
  }
}

Edge Player::calculateEdge(Direction direction, float topLeftX, float topLeftY, float directionOffset) const
{
  switch (direction)
  {
    case Direction::NORTH:
      return { topLeftX, topLeftY, topLeftX + directionOffset * PLAYER_SPRITE_WIDTH, topLeftY };
    case Direction::SOUTH:
      return { topLeftX, topLeftY + PLAYER_SPRITE_WIDTH, topLeftX + directionOffset * PLAYER_SPRITE_WIDTH, topLeftY + PLAYER_SPRITE_WIDTH };
    case Direction::EAST:
      return { topLeftX + PLAYER_SPRITE_WIDTH, topLeftY, topLeftX + PLAYER_SPRITE_WIDTH, topLeftY + directionOffset * PLAYER_SPRITE_WIDTH };
    case Direction::WEST:
      return { topLeftX, topLeftY, topLeftX, topLeftY + directionOffset * PLAYER_SPRITE_WIDTH };   
  }
}