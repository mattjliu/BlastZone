#include "raylib.h"
#include "BlastZone.h"
#include "Edge.h"
#include "BlastRay.h"
#include "LineSegment.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <utility>
#include <tuple>

BlastZone::BlastZone(float rayDeviance, float radius)
  : rayDeviance(rayDeviance), radius(radius)
{}

void BlastZone::drawBlastZone(float originX, float originY, const std::vector<Edge>& edgeMap, float alpha)
{
  convertEdgeMapToBlastZone(originX, originY, edgeMap);
  if (blastZonePolygonPoints.size() > 1)
  {
    # pragma omp parallel for
    for (std::size_t i = 0; i < blastZonePolygonPoints.size(); i++)
    {
      drawTriangleInCorrectOrder(
        originX,
        originY,
        blastZonePolygonPoints[i % blastZonePolygonPoints.size()],
        blastZonePolygonPoints[(i+1) % blastZonePolygonPoints.size()],
        alpha
      );
    }
  }
}

bool BlastZone::isPlayerInBlastZone(float originX, float originY, const Player& player) const
{
  std::array<Edge,4> playerEdges = player.getEdges();
  bool playerHit = false;
  if (blastZonePolygonPoints.size() > 1)
  {
    #pragma omp parallel for shared(playerHit)
    for (std::size_t i = 0; i < blastZonePolygonPoints.size(); i++)
    {
      if (!playerHit)
      {
        if (isPlayerContainedInBlastZone(originX, originY, blastZonePolygonPoints[i % blastZonePolygonPoints.size()], blastZonePolygonPoints[(i+1) % blastZonePolygonPoints.size()], player))
          playerHit = true;
        if (isPlayerIntersectsWithBlastZone(originX, originY, blastZonePolygonPoints[i % blastZonePolygonPoints.size()], playerEdges))
          playerHit = true;
      }
    }
  }
  return playerHit;
}

bool BlastZone::isPlayerContainedInBlastZone(float originX, float originY, BlastRay ray1, BlastRay ray2, const Player& player) const
{
  return CheckCollisionPointTriangle(
    { player.getPositionX(), player.getPositionY() },
    { originX, originY },
    { ray1.x, ray1.y },
    { ray2.x, ray2.y }
  );
}

bool BlastZone::isPlayerIntersectsWithBlastZone(float originX, float originY, BlastRay ray, const std::array<Edge,4>& playerEdges) const
{
  LineSegment rayLineSegment = { originX, originY, ray.x - originX, ray.y - originY };
  for (auto& edge : playerEdges)
  {
    LineSegment playerEdgeLineSegment = { edge.startX, edge.startY, edge.endX - edge.startX, edge.endY - edge.startY };
    if (std::fabs(playerEdgeLineSegment.dx - rayLineSegment.dx) > 0.0f && std::fabs(playerEdgeLineSegment.dy - rayLineSegment.dy) > 0.0f)
    {
      float t1, t2;
      std::tie(t1, t2) = getIntersectionPoints(rayLineSegment, playerEdgeLineSegment);
      if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1)
        return true;
    }
  }
  return false;
}

void BlastZone::convertEdgeMapToBlastZone(float originX, float originY, const std::vector<Edge>& edgeMap)
{
  blastZonePolygonPoints.clear();

  #pragma omp parallel for
  for (std::size_t i = 0; i < edgeMap.size(); i++)
  {
    Edge e = edgeMap[i];
    for (int j = 0; j < 2; j++) // Loop for start and end point of each edge
    {
      float ray_dx = (j == 0 ? e.startX : e.endX) - originX;
      float ray_dy = (j == 0 ? e.startY : e.endY) - originY;
      float baseAngle = std::atan2(ray_dy, ray_dx);

      updateBlastZonePolygonPoints(originX, originY, baseAngle - rayDeviance, edgeMap);
      updateBlastZonePolygonPoints(originX, originY, baseAngle, edgeMap);
      updateBlastZonePolygonPoints(originX, originY, baseAngle + rayDeviance, edgeMap);
    }
  }
  std::sort(
    blastZonePolygonPoints.begin(), blastZonePolygonPoints.end(),
    [](const BlastRay& r1, const BlastRay& r2)
    {
      return r1.angle < r2.angle;
    });

  auto uniquePointIterator = std::unique(
    blastZonePolygonPoints.begin(),
    blastZonePolygonPoints.end(),
    [](const BlastRay& r1, const BlastRay& r2)
    {
      return std::fabs(r1.x - r2.x) < 0.1f && std::fabs(r1.y - r2.y) < 0.1f; 
    });

  blastZonePolygonPoints.resize(std::distance(blastZonePolygonPoints.begin(), uniquePointIterator));
}

void BlastZone::updateBlastZonePolygonPoints(float ray_startX, float ray_startY, float angle, const std::vector<Edge>& edgeMap)
{
  float ray_dx = radius * std::cos(angle);
  float ray_dy = radius * std::sin(angle);

  float min_t1 = INFINITY;
  float pxAtClosest = 0, pyAtClosest = 0, angleAtClosest = 0;
  bool intersectionOccurs = false;

  for (const auto& edge : edgeMap)
  {
    float edge_dx = edge.endX - edge.startX;
    float edge_dy = edge.endY - edge.startY;

    if (std::fabs(edge_dx - ray_dx) > 0.0f && std::fabs(edge_dy - ray_dy) > 0.0f)
    {
      LineSegment edgeLineSegment = { edge.startX, edge.startY, edge_dx, edge_dy };
      LineSegment rayLineSegment = { ray_startX, ray_startY, ray_dx, ray_dy };
      float t1, t2;
      std::tie(t1, t2) = getIntersectionPoints(rayLineSegment, edgeLineSegment);

      if (t1 >= 0 && t2 >= 0 && t2 <= 1)
      {
        if (t1 < min_t1)
        {
          min_t1 = t1;
          pxAtClosest = ray_startX + ray_dx * t1;
          pyAtClosest = ray_startY + ray_dy * t1;
          angleAtClosest = std::atan2(pyAtClosest - ray_startY, pxAtClosest - ray_startX);
          intersectionOccurs = true;
        }
      }
    }
  }

  if (intersectionOccurs)
  {
    #pragma omp critical(addRayToBlastZone)
    {
      blastZonePolygonPoints.push_back({ angleAtClosest, pxAtClosest, pyAtClosest });
    }
  }
}

std::pair<float, float> BlastZone::getIntersectionPoints(LineSegment l1, LineSegment l2) const
{
  float t1 = (l2.dx * (l1.startY - l2.startY) - l2.dy * (l1.startX - l2.startX)) / (l1.dx * l2.dy - l1.dy * l2.dx);
  float t2 = (l1.dx * (l1.startY - l2.startY) - l1.dy * (l1.startX - l2.startX)) / (l1.dx * l2.dy - l1.dy * l2.dx);
  return std::make_pair(t1, t2);
}

bool BlastZone::getWindingOrder(float originX, float originY, BlastRay ray1, BlastRay ray2) const
{
  float vector1_x = ray1.x - originX;
  float vector1_y = ray1.y - originY;
  float vector2_x = ray2.x - originX;
  float vector2_y = ray2.y - originY;
  float crossProduct = vector1_x * vector2_y - vector2_x * vector1_y;
  return crossProduct > 0;
}

void BlastZone::drawTriangleInCorrectOrder(float originX, float originY, BlastRay ray1, BlastRay ray2, float alpha) const
{
  if (!getWindingOrder(originX, originY, ray1, ray2))
  {
    #pragma omp critical(drawTriangle)
    {
      DrawTriangle(
        {originX, originY}, 
        {ray1.x, ray1.y},
        {ray2.x, ray2.y},
        Fade(RAYWHITE, alpha)
      );
    }
  }
  else
  {
    #pragma omp critical(drawTriangle)
    {
      DrawTriangle(
        {originX, originY}, 
        {ray2.x, ray2.y},
        {ray1.x, ray1.y},
        Fade(RAYWHITE, alpha)
      );
    }
  }
}