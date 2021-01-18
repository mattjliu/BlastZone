#include "raylib.h"
#include "Level.h"
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <ctime>
#include <cstdlib>

Level::Level(int nTilesWidth, int nTilesHeight, int tileSize)
  : nTilesWidth(nTilesWidth), nTilesHeight(nTilesHeight), tileSize(tileSize),
    timeSinceLastSpawn(0), bombSpawnCount(0), bombDetonatedCount(0)
{
  std::srand(std::time(NULL));
  spawnProbability = MIN_SPAWN_PROBABILITY;
  spawnDelay = INITIAL_SPAWN_DELAY;
  tileSprite = LoadTexture(TILE_SPRITE_PATH.c_str());
  tileCount = nTilesWidth * nTilesHeight;
  createTileMap();
}

Level::~Level()
{
  UnloadTexture(tileSprite);
}

bool Level::addTileToMap(int xPosition, int yPosition)
{
  int cellIndex = coordinateToCellIndex(xPosition, yPosition);
  if (isBorderIndex(cellIndex))
    return false;

  if (tileMap[cellIndex].exists())
    tileMap[cellIndex].remove();
  else
    tileMap[cellIndex].place();

  convertTileMapToEdgeMap();

  return true;
}

void Level::addBombToMap(Bomb* bomb)
{
  bombField.addBomb(bomb);
  bombSpawnCount++;
}

bool Level::cellExistsAtCoordinate(int xPosition, int yPosition) const
{
  return tileMap[coordinateToCellIndex(xPosition, yPosition)].exists();
}

std::vector<Cell> Level::getTileMap() const
{
  return tileMap;
}

int Level::getTileCount() const
{
  return tileCount;
}

int Level::getTileSize() const
{
  return tileSize;
}

int Level::getNumberOfTilesWidth() const
{
  return nTilesWidth;
}

int Level::getNumberOfTilesHeight() const
{
  return nTilesHeight;
}

std::vector<Edge> Level::getEdgeMap() const
{
  return edgeMap;
}

int Level::getBombSpawnCount() const
{
  return bombSpawnCount;
}

int Level::getBombDetonatedCount() const
{
  return bombDetonatedCount;
}

void Level::drawMap()
{
  #pragma omp parallel for
  for (std::size_t i = 0; i < tileMap.size(); i++)
  {
    if (tileMap[i].exists())
    {
      #pragma omp critical(drawLevelTile)
      {
        DrawTextureRec(tileSprite, { static_cast<float>(tileSize), 0, static_cast<float>(tileSize), static_cast<float>(tileSize) }, { getCellX(i), getCellY(i) }, RAYWHITE);
      }
    }
    else
    {
      #pragma omp critical(drawLevelTile)
      {
        DrawTextureRec(tileSprite, { 0, 0, static_cast<float>(tileSize), static_cast<float>(tileSize) }, { getCellX(i), getCellY(i) }, RAYWHITE);
      }
    }
  }
}

bool Level::drawBombs(const Player& player)
{
  return bombField.drawBombs(edgeMap, player);
}

bool Level::updateBombs(float frameTime, const Player& player)
{
  bool bombDetonated = bombField.update(frameTime);
  timeSinceLastSpawn += frameTime;
  if (timeSinceLastSpawn > spawnDelay)
  {
    if ((static_cast<float>(std::rand()) / RAND_MAX) < spawnProbability)
    {
      spawnBombNextToPlayer(player);
      spawnProbability = MIN_SPAWN_PROBABILITY;
    }
    else
    {
      spawnRandomBomb();
      spawnProbability += SPAWN_PROBABILITY_UPDATE;
    }
    timeSinceLastSpawn -= spawnDelay; 
    updateSpawnDelay();
  }
  if (bombDetonated)
    bombDetonatedCount++;
  return bombDetonated;
}

void Level::createTileMap()
{ 
  tileMap.clear();
  tileMap.resize(tileCount);
  for (std::size_t i = 0; i < tileMap.size(); i++)
  {
    tileMap[i].setCoordinates(getCellX(i), getCellY(i));
    if (isOutOfBoundsIndex(i))
      continue;
    if (isBorderIndex(i))
      tileMap[i].place();
    else if ((static_cast<float>(std::rand()) / RAND_MAX) < CELL_PROBABILITY)
      tileMap[i].place();
  }
  convertTileMapToEdgeMap();
}

void Level::convertTileMapToEdgeMap()
{
  edgeMap.clear();

  for (std::size_t i = 0; i < tileMap.size(); i++)
    tileMap[i].clearAllEdges();

  for (int x = 1; x < nTilesWidth - 1; x++)
  {
    for (int y = 1; y < nTilesHeight - 1; y++)
    {
      int currentIndex = calculateCellIndex(x, y);
      if (tileMap[currentIndex].exists())
      {
        for (Direction direction : ALL_DIRECTIONS)
        {
          if (!tileMap[calculateNeighborIndex(direction, currentIndex)].exists())
            addEdgeToMap(direction, currentIndex);
        }       
      }
    }
  }
}

int Level::coordinateToCellIndex(int xPosition, int yPosition) const
{
  int cellsIndexDown = (yPosition / tileSize) * nTilesWidth;
  int cellsIndexAcross = xPosition / tileSize;
  return cellsIndexDown + cellsIndexAcross;
}

int Level::getCellX(int cellIndex) const
{
  return getCellColumn(cellIndex) * tileSize;
}

int Level::getCellY(int cellIndex) const
{
  return getCellRow(cellIndex) * tileSize;
}

int Level::getCellX(int xPosition, int yPosition) const
{
  return getCellColumn(coordinateToCellIndex(xPosition, yPosition)) * tileSize;
}

int Level::getCellY(int xPosition, int yPosition) const
{
  return getCellRow(coordinateToCellIndex(xPosition, yPosition)) * tileSize;
}

int Level::getCellRow(int cellIndex) const
{
  return (cellIndex / nTilesWidth);
}

int Level::getCellColumn(int cellIndex) const
{
  return (cellIndex % nTilesWidth);
}

bool Level::isBorderIndex(int cellIndex) const
{
  if (getCellRow(cellIndex) == 0 || getCellRow(cellIndex) == nTilesHeight - 1)
    return false;
  else if (getCellColumn(cellIndex) == 0 || getCellColumn(cellIndex) == nTilesWidth - 1)
    return false;
  else if (getCellRow(cellIndex) == 1 || getCellRow(cellIndex) == nTilesHeight - 2)
    return true;
  else if (getCellColumn(cellIndex) == 1 || getCellColumn(cellIndex) == nTilesWidth - 2)
    return true;
  else
    return false;
}

bool Level::isOutOfBoundsIndex(int cellIndex) const
{
  if (getCellRow(cellIndex) == 0 || getCellRow(cellIndex) == nTilesHeight - 1)
    return true;
  else if (getCellColumn(cellIndex) == 0 || getCellColumn(cellIndex) == nTilesWidth - 1)
    return true;
  else
    return false;
}

int Level::calculateCellIndex(int x, int y) const
{
  return y * nTilesWidth + x;
}

int Level::calculateNeighborIndex(Direction direction, int currentIndex) const
{
  switch (direction)
  {
    case Direction::NORTH:
      return currentIndex - nTilesWidth;
    case Direction::SOUTH:
      return currentIndex + nTilesWidth;
    case Direction::EAST:
      return currentIndex + 1;
    case Direction::WEST:
      return currentIndex - 1;
    default:
      std::cout << "Direction not valid!" << std::endl;
      exit(EXIT_FAILURE);
  }
}

void Level::checkEdge(Direction direction, int currentIndex)
{
  int indexOfTileInDirection = calculateNeighborIndex(direction, currentIndex); 
  if (!tileMap[indexOfTileInDirection].exists())
  {
    addEdgeToMap(direction, currentIndex);
  }
}

void Level::addEdgeToMap(Direction direction, int cellIndex)
{
  if (direction == Direction::WEST || direction == Direction::EAST)
  {
    int northIndex = calculateNeighborIndex(Direction::NORTH, cellIndex);
    Cell northernNeighbor = tileMap[northIndex];

    if (northernNeighbor.edgeExists(direction))
    {
      edgeMap[northernNeighbor.getEdgeID(direction)].endY += tileSize;
      tileMap[cellIndex].addEdge(direction);
      tileMap[cellIndex].setEdgeID(direction, northernNeighbor.getEdgeID(direction));      
    }
    else
    {
      Edge newEdge;

      if (direction == Direction::WEST)
      {
        newEdge.startX = tileMap[cellIndex].getX();
      }
      else if (direction == Direction::EAST)
      {
        newEdge.startX = tileMap[cellIndex].getX() + tileSize;
      }

      newEdge.startY = tileMap[cellIndex].getY();
      newEdge.endX = newEdge.startX; newEdge.endY = newEdge.startY + tileSize; 
      
      int edgeID = edgeMap.size();
      edgeMap.push_back(newEdge);

      tileMap[cellIndex].addEdge(direction);
      tileMap[cellIndex].setEdgeID(direction, edgeID);   
    }
  }
  else if (direction == Direction::NORTH || direction == Direction::SOUTH)
  {
    int westIndex = calculateNeighborIndex(Direction::WEST, cellIndex);
    Cell westernNeighbor = tileMap[westIndex];
    
    if (westernNeighbor.edgeExists(direction))
    {
      edgeMap[westernNeighbor.getEdgeID(direction)].endX += tileSize;
      tileMap[cellIndex].addEdge(direction);
      tileMap[cellIndex].setEdgeID(direction, westernNeighbor.getEdgeID(direction));
    }
    else
    {
      Edge newEdge;

      if (direction == Direction::NORTH)
      {
        newEdge.startY = tileMap[cellIndex].getY();
      }
      else if (direction == Direction::SOUTH) 
      {
        newEdge.startY = tileMap[cellIndex].getY() + tileSize;
      }

      newEdge.startX = tileMap[cellIndex].getX();
      newEdge.endX = newEdge.startX + tileSize; newEdge.endY = newEdge.startY;

      int edgeID = edgeMap.size();
      edgeMap.push_back(newEdge);

      tileMap[cellIndex].addEdge(direction);
      tileMap[cellIndex].setEdgeID(direction, edgeID); 
    }
  }
}

void Level::spawnRandomBomb()
{
  std::vector<int> emptyCellIndices = getEmptyCellIndices();
  int randomIndex = emptyCellIndices[std::rand() % emptyCellIndices.size()];
  int cellPositionX = getCellX(randomIndex);
  int cellPositionY = getCellY(randomIndex);
  int randomOffsetX = std::rand() % (tileSize - 10) + 5;
  int randomOffsetY = std::rand() % (tileSize - 10) + 5;
  int randomPositionX = cellPositionX + randomOffsetX;
  int randomPositionY = cellPositionY + randomOffsetY;

  Bomb* newBomb = new Bomb(randomPositionX, randomPositionY, 1.5f, 3.0f);
  addBombToMap(newBomb);
}

void Level::spawnBombNextToPlayer(const Player& player)
{
  int playerPositionX = static_cast<int>(player.getPositionX());
  int playerPositionY = static_cast<int>(player.getPositionY());
  int cellPositionX = getCellX(playerPositionX, playerPositionY);
  int cellPositionY = getCellY(playerPositionX, playerPositionY);
  int randomOffsetX = std::rand() % (tileSize - 10) + 5;
  int randomOffsetY = std::rand() % (tileSize - 10) + 5;
  int randomPositionX = cellPositionX + randomOffsetX;
  int randomPositionY = cellPositionY + randomOffsetY;

  Bomb* newBomb = new Bomb(randomPositionX, randomPositionY, 1.5f, 3.0f);
  addBombToMap(newBomb);
}

std::vector<int> Level::getEmptyCellIndices() const
{
  std::vector<int> emptyCellIndices(tileMap.size());
  std::size_t last = 0;
  for (std::size_t i = 0; i < tileMap.size(); i++)
  {
    if (!tileMap[i].exists() && !isOutOfBoundsIndex(i))
    {
      emptyCellIndices[last++] = i;
    }
  }
  emptyCellIndices.erase(emptyCellIndices.begin() + last, emptyCellIndices.end());
  return emptyCellIndices;
}

void Level::updateSpawnDelay()
{
  spawnDelay = spawnDelay > (MIN_SPAWN_DELAY + SPAWN_DELAY_UPDATE) ? spawnDelay - SPAWN_DELAY_UPDATE : MIN_SPAWN_DELAY;
}

void Level::updateSpawnProbability()
{
  spawnProbability = spawnProbability < (MAX_SPAWN_PROBABILITY - SPAWN_PROBABILITY_UPDATE) ? spawnProbability + SPAWN_PROBABILITY_UPDATE : MAX_SPAWN_PROBABILITY;
}

std::pair<float, float> Level::getSpawnLocation(float playerWidth) const
{
  std::vector<int> emptyCellIndices = getEmptyCellIndices();
  int randomSpawnIndex = std::rand() % static_cast<int>(emptyCellIndices.size());
  int cellPositionX = getCellX(emptyCellIndices[randomSpawnIndex]);
  int cellPositionY = getCellY(emptyCellIndices[randomSpawnIndex]);
  float offset = (tileSize - playerWidth) / 2.0f;
  return std::pair<float, float>(cellPositionX + offset, cellPositionY + offset);
}

void Level::generateNewLevel()
{
  timeSinceLastSpawn = 0;
  bombSpawnCount = 0;
  bombDetonatedCount = 0;
  spawnDelay = INITIAL_SPAWN_DELAY;
  spawnProbability = MIN_SPAWN_PROBABILITY;
  bombField.clearBombField();
  createTileMap();
}