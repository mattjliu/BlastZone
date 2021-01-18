#include "raylib.h"
#include "ShakyCam.h"
#include <cstdlib>
#include <cmath>

ShakyCam::ShakyCam(Vector2 offset, Vector2 target, float rotation, float zoom)
  : trauma(0.0f)
{
  baseCam = { offset, target, rotation, zoom };
  shakyCam = { offset, target, rotation, zoom };
}

void ShakyCam::update(float frameTime)
{
  trauma = trauma > frameTime ? trauma - frameTime : 0.0f;
  if (trauma > 0.0f)
    shakeCamera();
  else
    resetCamera();
}

void ShakyCam::shakeCamera()
{
  float randomRotation = 2.5 * std::pow(trauma, 2) * (2 * ((float) std::rand() / RAND_MAX) - 1);
  float randomXOffset = 1 * std::pow(trauma, 2) * (2 * ((float) std::rand() / RAND_MAX) - 1);
  float randomYOffset = 1 * std::pow(trauma, 2) * (2 * ((float) std::rand() / RAND_MAX) - 1);
  shakyCam.rotation = baseCam.rotation + randomRotation;
  shakyCam.target = { baseCam.target.x + randomXOffset, baseCam.target.y + randomYOffset };
}

void ShakyCam::resetCamera()
{
  shakyCam = { baseCam.offset, baseCam.target, baseCam.rotation, baseCam.zoom };
}

void ShakyCam::addTrauma(float traumaAmount)
{
  trauma += traumaAmount; 
}

const Camera2D& ShakyCam::getShakyCam() const
{
  return shakyCam;
}