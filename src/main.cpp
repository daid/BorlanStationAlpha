#include <cstdio>
#include <cassert>
#include "frontend/frontend.h"
#include "cell.h"
#include "map.h"
#include "ecs.h"
#include "mapgen.h"
#include "components.h"

#include "system/vision.h"
#include "system/oxygen.h"


ECS engine;

bool actionMove(ECS::Entity e, Vector2i offset)
{
    auto position = e.get<Position>() + offset;
    //if (map.hasSolidEntity(position))
    //    return false;
    e.remove<Position>();
    e.set(Position{position});
    return true;
}

int main(int argc, char** argv)
{
    Mapgen();

    auto player = engine.create();
    player.set(Position{{12, 4}}).set(Visual{'@', HsvColor(0, 0, 100), 10}).set<Player>().set<Solid>();
    player.set(Light{15, HsvColor(0, 0, 70)});

    Frontend frontend;

    while(1) {
        runOxygenSystem();
        runVisionSystem();

        Vector2i camera_position = player.get<Position>();

        auto size = frontend.beginDrawing();
        camera_position -= size / 2;

        for(int y=std::max(0, -camera_position.y); y<std::min(size.y, map.size().y - camera_position.y); y++) {
            for(int x=std::max(0, -camera_position.x); x<std::min(size.x, map.size().x - camera_position.x); x++) {

                Cell& cell = map(camera_position + Vector2i(x, y));
                if (cell.visible && (cell.light_level.r > 0.1 || cell.light_level.g > 0.1 || cell.light_level.b > 0.1)) {
                    frontend.setbg(x, y, cell.light_level * 0.1f);
                    cell.last_seen_as = ' ';
                    if (cell.floor) {
                        frontend.setbg(x, y, cell.light_level * 0.3f);
                        frontend.draw(x, y, '.', cell.light_level);
                        cell.last_seen_as = '.';
                    }
                    
                    int shown_priority = std::numeric_limits<int>::min();
                    for(auto e : cell.entities) {
                        if (e.has<Visual>()) {
                            auto visual = e.get<Visual>();
                            if (visual.priority > shown_priority)
                            {
                                frontend.draw(x, y, visual.c, Color(visual.color) * cell.light_level);
                                cell.last_seen_as = visual.c;
                                shown_priority = visual.priority;
                            }
                        }
                    }
                } else {
                    frontend.draw(x, y, cell.last_seen_as, Color(0.2, 0.2, 0.2));
                }

                //frontend.setbg(x, y, Color(1, 1, 1) * cell.oxygen);
            }
        }

        frontend.present();

        auto input = frontend.getInput();
        switch(input)
        {
        case INPUT_QUIT: return 0;
        case INPUT_RIGHT: actionMove(player, Vector2i{1, 0}); break;
        case INPUT_LEFT: actionMove(player, Vector2i{-1, 0}); break;
        case INPUT_DOWN: actionMove(player, Vector2i{0, 1}); break;
        case INPUT_UP: actionMove(player, Vector2i{0, -1}); break;
        }

        //for(auto e : map(player.get<Position>()).entities) { if (e.has<Solid>()) e.destroy(); }
    }
    
    return 0;
}
