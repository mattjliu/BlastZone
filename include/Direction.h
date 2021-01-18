#pragma once
#include <array>

enum class Direction
{
  NORTH, SOUTH, EAST, WEST
};

const std::array<Direction, 4> ALL_DIRECTIONS = {
  Direction::NORTH, Direction::SOUTH, Direction::EAST, Direction::WEST
};