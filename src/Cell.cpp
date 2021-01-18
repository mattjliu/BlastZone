#include "raylib.h"
#include "Cell.h"

Cell::Cell()
  : color(BLUE), existsFlag(false)
{
}

bool Cell::exists() const
{
  return existsFlag;
}

void Cell::place()
{
  existsFlag = true;
}

void Cell::remove()
{
  existsFlag = false;
}

bool Cell::edgeExists(Direction direction) const
{
  return edgeExistsArray[static_cast<int>(direction)];
}

void Cell::addEdge(Direction direction)
{
  edgeExistsArray[static_cast<int>(direction)] = true;
}

void Cell::removeEdge(Direction direction)
{
  edgeExistsArray[static_cast<int>(direction)] = false;
}

int Cell::getEdgeID(Direction direction) const
{
  return edgeIDArray[static_cast<int>(direction)];
}

void Cell::setEdgeID(Direction direction, int id)
{
  edgeIDArray[static_cast<int>(direction)] = id;
}

void Cell::clearAllEdges()
{
  for (Direction direction : ALL_DIRECTIONS)
  {
    removeEdge(direction);
    setEdgeID(direction, 0);
  }
}

void Cell::setCoordinates(int x, int y)
{
  this->x = x;
  this->y = y;
}

int Cell::getX() const
{
  return x;
}

int Cell::getY() const
{
  return y;
}