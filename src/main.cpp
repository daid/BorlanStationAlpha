#include <cstdio>
#include <cassert>
#include "frontend/frontend.h"
#include "cell.h"
#include "map.h"
#include "ecs.h"
#include "mapgen.h"
#include "components.h"
#include "messagelog.h"

#include "system/vision.h"
#include "system/oxygen.h"

#include "view/map.h"
#include "view/hud.h"


ECS engine;

bool action_move(ECS::Entity e, Vector2i offset)
{
    auto position = e.get<Position>() + offset;
    if (e.has<Solid>() && map.has_solid_entity(position))
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
    player.set(Health{20, 40});
    //player.set<Solid>();
    player.set(Light{10, HsvColor(0, 0, 50)});

    Frontend frontend;
    MapView map_view;
    HudView hud_view;

    for(int n=0; n<20; n++)
        mlog.add("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris at mattis nunc, eget pellentesque turpis. Interdum et malesuada fames ac ante ipsum primis in faucibus. Curabitur elementum ipsum vel tortor semper bibendum. Ut molestie mattis lacus, consectetur scelerisque elit eleifend vitae. Maecenas efficitur fringilla lectus, a eleifend justo cursus vel. Maecenas eget arcu hendrerit, sagittis erat nec, sagittis dui. Cras risus erat, porttitor vel rutrum sed, lacinia at lacus.");
    mlog.add("Test12345");

    while(1) {
        run_oxygen_system();
        run_vision_system();

        while(mlog.has_new_messages())
            mlog.confirm_new_message();

        frontend.begin_drawing();
        map_view.draw(frontend);
        hud_view.draw(frontend);
        frontend.present();

        auto input = frontend.get_input();
        switch(input)
        {
        case INPUT_QUIT: return 0;
        case INPUT_RIGHT: action_move(player, Vector2i{1, 0}); break;
        case INPUT_LEFT: action_move(player, Vector2i{-1, 0}); break;
        case INPUT_DOWN: action_move(player, Vector2i{0, 1}); break;
        case INPUT_UP: action_move(player, Vector2i{0, -1}); break;
        }

        //for(auto e : map(player.get<Position>()).entities) { if (e.has<Solid>() && !e.has<Player>()) e.destroy(); }
    }
    
    return 0;
}
