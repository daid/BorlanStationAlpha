#include "vision.h"
#include "components.h"
#include "map.h"

void runVisionSystem()
{
    for(Cell& cell : map) {
        cell.visible = false;
        cell.light_level = Color{};

        cell.visible = true;
        cell.light_level = Color{0.3,0.3,0.3};
    }
    for(auto&& [e, light, position] : ecs.query<Light, Position>()) {
        map.visitFieldOfView(position, light.distance, [&light, &position](Vector2i p) {
            if (map.visionBlocked(p) && p != position) return;
            auto dist = (Vector2f(p) - Vector2f(position)).length();
            auto f = std::min(0.5f, (light.distance - dist) / (light.distance * 0.5f));
            if (std::abs(p.x - position.x) == std::abs(p.y - position.y) && position != p) f *= 0.5f;
            map(p).light_level += light.color * f;
        });
    }
    for(auto&& [e, player, position] : ecs.query<Player, Position>()) {
        map.visitFieldOfView(position, 100, [&position](Vector2i p) {
            map(p).visible = true;
            if (map.visionBlocked(p))
            {
                auto dir = position - p;
                if (dir.x) dir.x = std::copysign(1, dir.x);
                if (dir.y) dir.y = std::copysign(1, dir.y);
                if (dir.x && dir.y) {
                    if (std::abs(position.x - p.x) < std::abs(position.y - p.y)) {
                        if (!map.visionBlocked(p + Vector2i{0, dir.y})) dir.x = 0;
                    } else {
                        if (!map.visionBlocked(p + Vector2i{dir.x, 0})) dir.y = 0;
                    }
                }
                map(p).light_level = map(p + dir).light_level;
            }
        });
    }
}