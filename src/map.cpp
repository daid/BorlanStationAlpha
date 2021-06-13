#include "map.h"
#include <cstdio>

Map map;


void Map::visit_field_of_view(Vector2i center, int radius, const std::function<void(Vector2i)>& callback)
{
    callback(center);
    fov_step(center, {1, 0}, radius, 1, -1.0, 1.0, callback);
    fov_step(center, {0, 1}, radius, 1, -1.0, 1.0, callback);
    fov_step(center, {-1, 0}, radius, 1, -1.0, 1.0, callback);
    fov_step(center, {0, -1}, radius, 1, -1.0, 1.0, callback);
}

float Map::line_of_sight(Vector2i start, Vector2i end)
{
    Vector2i delta = end - start;
    Vector2i i{delta.x > 0 ? 1 : -1, delta.y > 0 ? 1 : -1};
    delta = {std::abs(delta.x), std::abs(delta.y)};
    if (delta.x > delta.y) {
        int open_count = 0;
        for(int err_start=0; err_start<delta.x; err_start++) {
            int err = err_start;
            int y = start.y;
            bool blocked = false;
            for(int x=start.x; x != end.x; x+=i.x) {
                if (has_entity_with<BlockVision>({x, y})) {
                    blocked = true;
                    break;
                }
                err -= delta.y;
                if (err < 0) {
                    y += i.y;
                    err += delta.x;
                }
            }
            if (!blocked)
                open_count++;
        }
        return float(open_count) / float(delta.x);
    } else {
        int open_count = 0;
        for(int err_start=0; err_start<delta.y; err_start++) {
            int err = err_start;
            int x = start.x;
            bool blocked = false;
            for(int y=start.y; y != end.y; y+=i.y) {
                if (has_entity_with<BlockVision>({x, y})) {
                    blocked = true;
                    break;
                }
                err -= delta.x;
                if (err < 0) {
                    x += i.x;
                    err += delta.y;
                }
            }
            if (!blocked)
                open_count++;
        }
        return float(open_count) / float(delta.y);
    }
}

void Map::fov_step(Vector2i center, Vector2i dir, int radius, int row, float fmin, float fmax, const std::function<void(Vector2i)>& callback)
{
    if (row > radius)
        return;
    if (fmax - fmin < 0.001)
        return;
    auto side = Vector2i{-dir.y, dir.x};
    for(int n=-row; n<=row; n++) {
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
        if (has_entity_with<BlockVision>(p)) {
            fov_step(center, dir, radius, row + 1, fmin, start, callback);
            fmin = end;
        }
    }

    fov_step(center, dir, radius, row + 1, fmin, fmax, callback);
}

bool Map::inside(Vector2i position) {
    return position.x >= 0 && position.y >= 0 && position.x < size().x && position.y < size().y;
}

void Position::on_add(ecs::EntityBase e)
{
    map(x, y).entities.insert(engine.upgrade(e));
}

void Position::on_remove(ecs::EntityBase e)
{
    map(x, y).entities.erase(engine.upgrade(e));
}
