#include "map.h"
#include <cstdio>

Map map;

bool Map::hasSolidEntity(Vector2i position)
{
    if (position.x < 0 || position.y < 0 || position.x >= size().x || position.y >= size().y)
        return true;
    for(auto e : map(position).entities)
        if (e.has<Solid>())
            return true;
    return false;
}

bool Map::visionBlocked(Vector2i position)
{
    if (position.x < 0 || position.y < 0 || position.x >= size().x || position.y >= size().y)
        return true;
    for(auto e : map(position).entities)
        if (e.has<BlockVision>())
            return true;
    return false;
}

void Map::visitFieldOfView(Vector2i center, int radius, const std::function<void(Vector2i)>& callback)
{
    callback(center);
    fovStep(center, {1, 0}, radius, 1, -1.0, 1.0, callback);
    fovStep(center, {0, 1}, radius, 1, -1.0, 1.0, callback);
    fovStep(center, {-1, 0}, radius, 1, -1.0, 1.0, callback);
    fovStep(center, {0, -1}, radius, 1, -1.0, 1.0, callback);
}

void Map::fovStep(Vector2i center, Vector2i dir, int radius, int row, float fmin, float fmax, const std::function<void(Vector2i)>& callback)
{
    if (row >= radius)
        return;
    if (fmax - fmin < 0.001)
        return;
    auto side = Vector2i{-dir.y, dir.x};
    for(int n=-row; n<=row; n++)
    {
        auto start = (float(n) - 0.5f) / (float(row) + 0.5f);
        auto end = (float(n) + 0.5f) / (float(row) + 0.5f);
        
        if (end < fmin)
            continue;
        if (start > fmax)
            continue;

        auto p = center + dir * row + side * n;
        if (p.x < 0 || p.y < 0 || p.x >= size().x || p.y >= size().y)
            continue;

        callback(p);
        if (visionBlocked(p))
        {
            fovStep(center, dir, radius, row + 1, fmin, start, callback);
            fmin = end;
        }
    }

    fovStep(center, dir, radius, row + 1, fmin, fmax, callback);
}

void Position::onCreate(EntityBase e)
{
    map(x, y).entities.insert(ecs.upgrade(e));
}

void Position::onDestroy(EntityBase e)
{
    map(x, y).entities.erase(ecs.upgrade(e));
}
