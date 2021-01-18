#pragma once
#include "raylib.h"

class ShakyCam
{
public:
  ShakyCam(Vector2, Vector2, float, float);
  void update(float);
  void shakeCamera();
  void resetCamera();
  void addTrauma(float=1.0f);
  const Camera2D& getShakyCam() const;
private:
  Camera2D baseCam;
  Camera2D shakyCam;
  float trauma;
};