#include "raylib.h"
#include "Level.h"
#include "Direction.h"
#include "Player.h"
#include "ShakyCam.h"
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <utility>

const int TILE_SIZE = 40;
const int SCREEN_WIDTH = 30 * TILE_SIZE;
const int SCREEN_HEIGHT = 20 * TILE_SIZE;

const Vector2 CAMERA_OFFSET = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
const Vector2 CAMERA_TARGET = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
const float CAMERA_ROTATION = 0.0f;
const float CAMERA_ZOOM = 1.0f;

const float PLAYER_VELOCITY = 100.0f;
const int PLAYER_WIDTH = 20.f;

bool gameLost = false;
float lossPlayerAlpha = 1.0f;
float lossScreenAlpha = 1.0f;
int bombsSurvived = -1;

void updatePlayer(Player* player, float delta, const Level& level);
bool drawGameState(Level* level, Player* player, bool gameLost, float lossPlayerAlpha);
void drawLossScreen(int SCREEN_WIDTH, int SCREEN_HEIGHT, float lossPlayerAlpha);
void resetGame(Level* level, Player* player);

int main(void)
{
  std::srand(std::time(NULL));

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Level Editor");
  InitAudioDevice(); 

  Level* level = new Level(SCREEN_WIDTH / TILE_SIZE, SCREEN_HEIGHT / TILE_SIZE, TILE_SIZE);
  std::pair<float, float> spawnLocation = level->getSpawnLocation(PLAYER_WIDTH);
  Player* player = new Player(PLAYER_VELOCITY, spawnLocation.first, spawnLocation.second);
  ShakyCam camera(CAMERA_OFFSET, CAMERA_TARGET, CAMERA_ROTATION, CAMERA_ZOOM);

  SetTargetFPS(60);

  while (!WindowShouldClose())
  {
    float frameTime = GetFrameTime();
    camera.update(frameTime);

    if (!gameLost)
      updatePlayer(player, frameTime, *level);
    else
    {
      lossPlayerAlpha = lossPlayerAlpha < frameTime ? 0.0f : lossPlayerAlpha - frameTime;
      if (lossPlayerAlpha == 0.0f)
        lossScreenAlpha = lossScreenAlpha < frameTime ? 0.0f : lossScreenAlpha - frameTime;
      if (IsKeyPressed(KEY_SPACE))
      {
        resetGame(level, player);
      }
    }

    if (level->updateBombs(frameTime, *player))
      camera.addTrauma();

    BeginDrawing();
    ClearBackground(GRAY);
    BeginMode2D(camera.getShakyCam());

    gameLost = drawGameState(level, player, gameLost, lossPlayerAlpha);

    DrawFPS(5, 10);
    DrawText(("Bombs Spawned: " + std::to_string(level->getBombSpawnCount())).c_str(), SCREEN_WIDTH - 200, 10, 20, RAYWHITE);

    EndMode2D();

    if (gameLost)
    {
      drawLossScreen(SCREEN_WIDTH, SCREEN_HEIGHT, lossPlayerAlpha);
      if (bombsSurvived == -1)
        bombsSurvived = level->getBombDetonatedCount() - 1;
    }

    EndDrawing();
  }

  CloseAudioDevice();
  CloseWindow();
  delete level;
  delete player;
  return 0;
}

void updatePlayer(Player* player, float delta, const Level& level)
{
  if (IsKeyDown(KEY_W))
    player->move(Direction::NORTH, delta, level);
  if (IsKeyDown(KEY_A))
    player->move(Direction::WEST, delta, level);
  if (IsKeyDown(KEY_S))
    player->move(Direction::SOUTH, delta, level);
  if (IsKeyDown(KEY_D))
    player->move(Direction::EAST, delta, level);
}

bool drawGameState(Level* level, Player* player, bool gameLost, float lossPlayerAlpha)
{
  level->drawMap();
  if (!gameLost)
    player->draw();
  else
    player->drawLoss(lossPlayerAlpha);

  if (level->drawBombs(*player))
    return gameLost ? gameLost : !gameLost;
  else
    return gameLost;
}

void drawLossScreen(int SCREEN_WIDTH, int SCREEN_HEIGHT, float lossPlayerAlpha)
{
  float screenAlpha = std::pow((1.0f - lossScreenAlpha), 3) / 2.0f;
  DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, screenAlpha));
  DrawText(("Bombs Survived: " + std::to_string(bombsSurvived)).c_str(), (SCREEN_WIDTH / 2) - 225, (SCREEN_HEIGHT / 2) - 50, 50, Fade(RAYWHITE, 1.0f - lossScreenAlpha));
}

void resetGame(Level* level, Player* player)
{
  level->generateNewLevel();
  std::pair<float, float> spawnLocation = level->getSpawnLocation(PLAYER_WIDTH);
  player->resetPlayer(spawnLocation.first, spawnLocation.second);
  gameLost = false;
  lossPlayerAlpha = 1.0f;
  lossScreenAlpha = 1.0f;
  bombsSurvived = -1;
}