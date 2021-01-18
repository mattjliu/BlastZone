#pragma once
#include "Edge.h"
#include "LineSegment.h"
#include "BlastRay.h"
#include "Player.h"
#include <vector>
#include <array>
#include <utility>

class BlastZone
{
public:
  BlastZone(float, float);
  void drawBlastZone(float, float, const std::vector<Edge>&, float alpha=1.0f);
  bool isPlayerInBlastZone(float, float, const Player&) const;
private:
  float rayDeviance;
  float radius;
  std::vector<BlastRay> blastZonePolygonPoints;
  void convertEdgeMapToBlastZone(float, float, const std::vector<Edge>&);
  void updateBlastZonePolygonPoints(float, float, float, const std::vector<Edge>&);
  std::pair<float, float> getIntersectionPoints(LineSegment, LineSegment) const;
  bool isPlayerContainedInBlastZone(float, float, BlastRay, BlastRay, const Player&) const;
  bool isPlayerIntersectsWithBlastZone(float, float, BlastRay, const std::array<Edge,4>&) const;
  bool getWindingOrder(float, float, BlastRay, BlastRay) const;
  void drawTriangleInCorrectOrder(float, float, BlastRay, BlastRay, float alpha) const;
};