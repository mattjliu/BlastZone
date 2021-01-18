#pragma once
#include "raylib.h"
#include "Direction.h"
#include <array>

class Cell
{
public:
  Color color;

  Cell();
  bool exists() const;
  void place();
  void remove();
  bool edgeExists(Direction) const;
  void addEdge(Direction);
  void removeEdge(Direction);
  int getEdgeID(Direction) const;
  void setEdgeID(Direction, int);
  void clearAllEdges();

  void setCoordinates(int, int);
  int getX() const;
  int getY() const;

private:
  bool existsFlag;
  int x;
  int y;
  std::array<bool, 4> edgeExistsArray;
  std::array<int, 4> edgeIDArray;
};