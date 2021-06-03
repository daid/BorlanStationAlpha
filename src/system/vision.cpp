#include "vision.h"
#include "components.h"
#include "map.h"


void run_vision_system()
{
    for(Cell& cell : map) {
        cell.visible = false;
        cell.light_level = Color{};

        //cell.visible = true;
        //cell.light_level = Color{0.3,0.3,0.3};
    }
    for(auto&& [e, light, position] : engine.query<Light, Position>()) {
        map.visit_field_of_view(position, light.distance, [&light=light, &position=position](Vector2i p) {
            if (map.vision_blocked(p) && p != position) return;
            auto dist = (Vector2f{p.x, p.y} - Vector2f{position.x, position.y}).length();
            auto f = std::min(1.0f, (light.distance - dist) / (light.distance * 0.5f));

            map(p).light_level = map(p).light_level.max(light.color * f);
        });
    }
    for(auto&& [e, position] : engine.query<Player, Position>()) {
        map.visit_field_of_view(position, 100, [&position=position](Vector2i p) {
            map(p).visible = true;
            if (map.vision_blocked(p))
            {
                auto dir = position - p;
                if (dir.x) dir.x = std::copysign(1, dir.x);
                if (dir.y) dir.y = std::copysign(1, dir.y);
                if (dir.x && dir.y) {
                    if (std::abs(position.x - p.x) < std::abs(position.y - p.y)) {
                        if (!map.vision_blocked(p + Vector2i{0, dir.y})) dir.x = 0;
                    } else {
                        if (!map.vision_blocked(p + Vector2i{dir.x, 0})) dir.y = 0;
                    }
                }
                map(p).light_level = map(p + dir).light_level;
            }
        });
    }
}
