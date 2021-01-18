#pragma once
#include "Direction.h"
#include "Cell.h"
#include "Edge.h"
#include "Bomb.h"
#include "BombField.h"
#include "Player.h"
#include <vector>
#include <utility>
#include <string>

class Level
{
public:
  Level(int, int, int);
  ~Level();
  bool cellExistsAtCoordinate(int, int) const;
  bool coordinateHasCell(int, int) const;
  std::vector<Cell> getTileMap() const;
  int getTileCount() const;
  int getTileSize() const;
  bool addTileToMap(int, int);
  void addBombToMap(Bomb* bomb);
  int getCellX(int, int) const;
  int getCellY(int, int) const;
  int getNumberOfTilesWidth() const;
  int getNumberOfTilesHeight() const;
  std::vector<Edge> getEdgeMap() const;
  int getBombSpawnCount() const;
  int getBombDetonatedCount() const;
  void drawMap();
  bool drawBombs(const Player&);
  bool updateBombs(float, const Player&);
  std::pair<float, float> getSpawnLocation(float) const;
  void generateNewLevel();
private:
  const std::string TILE_SPRITE_PATH = "res/tiles.png";
  const float MIN_SPAWN_PROBABILITY = 0.1f;
  const float MAX_SPAWN_PROBABILITY = 0.1f;
  const float SPAWN_PROBABILITY_UPDATE = 0.05f;
  const float MIN_SPAWN_DELAY = 1.5f;
  const float INITIAL_SPAWN_DELAY = 2.0f;
  const float SPAWN_DELAY_UPDATE = 0.01f;
  const float CELL_PROBABILITY = 0.1f;
  int bombSpawnCount;
  int bombDetonatedCount;
  int nTilesWidth;
  int nTilesHeight;
  int tileSize;
  int tileCount;
  float spawnDelay;
  float spawnProbability;
  float timeSinceLastSpawn;
  std::vector<Cell> tileMap;
  std::vector<Edge> edgeMap;
  BombField bombField;
  Texture2D tileSprite;
  int coordinateToCellIndex(int, int) const;
  void createTileMap();
  void convertTileMapToEdgeMap();
  int getCellX(int) const;
  int getCellY(int) const;
  int getCellRow(int) const;
  int getCellColumn(int) const;
  bool isBorderIndex(int) const;
  bool isOutOfBoundsIndex(int) const;

  int calculateCellIndex(int, int) const;
  int calculateNeighborIndex(Direction, int) const;
  void checkEdge(Direction, int);
  void addEdgeToMap(Direction, int);
  void spawnRandomBomb();
  void spawnBombNextToPlayer(const Player&);
  void updateSpawnDelay();
  void updateSpawnProbability();
  std::vector<int> getEmptyCellIndices() const;
};