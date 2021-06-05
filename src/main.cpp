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
#include "view/menu.h"


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

bool action_pick(ECS::Entity e)
{
    for(auto it : map(e.get<Position>()).entities) {
        auto ce = engine.upgrade(it);
        if (ce.has<Item>()) {
            ce.remove<Position>();
            ce.set(InInventory{e});
            return true;
        }
    }
    return false;
}

bool action_drop(ECS::Entity from, ECS::Entity target)
{
    target.remove<InInventory>();
    target.set(from.get<Position>());
    return true;
}

std::optional<ECS::Entity> select_from_inventory(Frontend& frontend, Inventory& inv, std::string_view title)
{
    std::vector<std::string> options;
    for(auto e : engine.upgrade(inv.contents)) {
        options.push_back(e.get<Item>().name);
    }
    if (options.empty()) {
        mlog.add("Got no items.");
        return {};
    }

    MenuView menu(title, options);
    auto res = menu.get_result(frontend);
    if (res.has_value()) {
        return engine.upgrade(inv.contents[res.value()]);
    }
    return {};
}

int main(int argc, char** argv)
{
    Mapgen();

    auto player = engine.create();
    player.set(Position{{12, 4}}).set(Visual{'@', HsvColor(0, 0, 100), 10}).set<Player>().set(Inventory{});
    player.set(Health{20, 40});
    //player.set<Solid>();
    player.set(Light{10, HsvColor(0, 0, 50)});

    Frontend frontend;
    MapView map_view;
    HudView hud_view;

    for(int n=0; n<20; n++)
        mlog.add("This is test message @", n);
    mlog.add("Test12345");

    while(mlog.has_new_messages())
        mlog.confirm_new_message();
    run_vision_system();

    while(1) {
        View::draw_views(frontend);

        auto input = frontend.get_input();
        bool did_action = false;
        if (!mlog.has_new_messages())
        {
            switch(input) {
            case INPUT_QUIT: return 0;
            case INPUT_RIGHT: did_action = action_move(player, Vector2i{1, 0}); break;
            case INPUT_LEFT: did_action = action_move(player, Vector2i{-1, 0}); break;
            case INPUT_DOWN: did_action = action_move(player, Vector2i{0, 1}); break;
            case INPUT_UP: did_action = action_move(player, Vector2i{0, -1}); break;
            case 'p': did_action = action_pick(player); break;
            case 'd': {
                auto res = select_from_inventory(frontend, player.get<Inventory>(), "Which item to drop?");
                if (res.has_value()) {
                    did_action = action_drop(player, res.value());
                }
            }break;
            }
            //for(auto e : map(player.get<Position>()).entities) { if (e.has<Solid>() && !e.has<Player>()) e.destroy(); }
        }

        if (did_action) {
            run_oxygen_system();
            run_vision_system();
        }
        if (did_action || input) {
            while(mlog.has_new_messages() && hud_view.msg_line_count > 0) {
                mlog.confirm_new_message();
                hud_view.msg_line_count--;
            }
        }
    }
    
    return 0;
}
