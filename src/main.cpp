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

#include "view/map.h"
#include "view/hud.h"


ECS engine;

bool actionMove(ECS::Entity e, Vector2i offset)
{
    auto position = e.get<Position>() + offset;
    if (e.has<Solid>() && map.hasSolidEntity(position))
        return false;
    e.remove<Position>();
    e.set(Position{position});
    return true;
}

int main(int argc, char** argv)
{
    Mapgen();

    auto player = engine.create();
    player.set(Position{{12, 4}}).set(Visual{'@', HsvColor(0, 0, 100), 10}).set<Player>();
    //player.set<Solid>();
    player.set(Light{10, HsvColor(0, 0, 50)});

    Frontend frontend;
    MapView map_view;
    HudView hud_view;

    while(1) {
        runOxygenSystem();
        runVisionSystem();

        frontend.beginDrawing();
        map_view.draw(frontend);
        hud_view.draw(frontend);
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
