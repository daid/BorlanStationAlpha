#include "map.h"
#include <cstdio>

Map map;

bool Map::has_solid_entity(Vector2i position)
{
    if (position.x < 0 || position.y < 0 || position.x >= size().x || position.y >= size().y)
        return true;
    for(auto e : map(position).entities)
        if (e.has<Solid>())
            return true;
    return false;
}

bool Map::vision_blocked(Vector2i position)
{
    if (position.x < 0 || position.y < 0 || position.x >= size().x || position.y >= size().y)
        return true;
    for(auto e : map(position).entities)
        if (e.has<BlockVision>())
            return true;
    return false;
}

void Map::visit_field_of_view(Vector2i center, int radius, const std::function<void(Vector2i)>& callback)
{
    callback(center);
    fov_step(center, {1, 0}, radius, 1, -1.0, 1.0, callback);
    fov_step(center, {0, 1}, radius, 1, -1.0, 1.0, callback);
    fov_step(center, {-1, 0}, radius, 1, -1.0, 1.0, callback);
    fov_step(center, {0, -1}, radius, 1, -1.0, 1.0, callback);
}

void Map::fov_step(Vector2i center, Vector2i dir, int radius, int row, float fmin, float fmax, const std::function<void(Vector2i)>& callback)
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
        if (vision_blocked(p))
        {
            fov_step(center, dir, radius, row + 1, fmin, start, callback);
            fmin = end;
        }
    }

    fov_step(center, dir, radius, row + 1, fmin, fmax, callback);
}

void Position::on_add(ecs::EntityBase e)
{
    map(x, y).entities.insert(engine.upgrade(e));
}

void Position::on_remove(ecs::EntityBase e)
{
    map(x, y).entities.erase(engine.upgrade(e));
}
