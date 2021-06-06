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
#include "system/ai.h"

#include "view/map.h"
#include "view/hud.h"
#include "view/menu.h"


ECS engine;
ecs::Systems<
    AISystem,
    OxygenSystem,
    VisionSystem
> systems;


int action_move(ECS::Entity e, Vector2i offset)
{
    auto position = e.get<Position>() + offset;
    if (e.has<Solid>() && map.has_solid_entity(position))
        return false;
    e.remove<Position>();
    e.set(Position{position});
    return 10;
}

int action_pick(ECS::Entity e)
{
    for(auto it : map(e.get<Position>()).entities) {
        auto ce = engine.upgrade(it);
        if (ce.has<Item>()) {
            ce.remove<Position>();
            ce.set(InInventory{e});
            return 5;
        }
    }
    return 0;
}

int action_drop(ECS::Entity from, ECS::Entity target)
{
    target.remove<InInventory>();
    target.set(from.get<Position>());
    return 5;
}

std::optional<ECS::Entity> select_from_inventory(Frontend& frontend, Inventory& inv, std::string_view title)
{
    std::vector<std::string> options;
    for(auto e : engine.upgrade(inv.contents)) {
        options.push_back(e.get<Name>());
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
    Mapgen mg;

    auto player = engine.create();
    player.set(Position{mg.start}).set(Visual{'@', HsvColor(0, 0, 100), 10}).set<Player>().set(Inventory{});
    player.set(Health{20, 40});
    player.set<Solid>();
    player.set(Light{10, HsvColor(0, 0, 50)});

    Frontend frontend;
    MapView map_view;
    HudView hud_view;

    mlog.add("Welcome to Borlan Station Alpha");

    while(mlog.has_new_messages())
        mlog.confirm_new_message();
    systems.turn();

    while(1) {
        View::draw_views(frontend);

        auto input = frontend.get_input();
        int execute_turns = 0;
        if (!mlog.has_new_messages()) {
            switch(input) {
            case INPUT_QUIT: return 0;
            case INPUT_RIGHT: execute_turns = action_move(player, Vector2i{1, 0}); break;
            case INPUT_LEFT: execute_turns = action_move(player, Vector2i{-1, 0}); break;
            case INPUT_DOWN: execute_turns = action_move(player, Vector2i{0, 1}); break;
            case INPUT_UP: execute_turns = action_move(player, Vector2i{0, -1}); break;
            case 'p': execute_turns = action_pick(player); break;
            case 'd': {
                auto res = select_from_inventory(frontend, player.get<Inventory>(), "Which item to drop?");
                if (res.has_value()) {
                    execute_turns = action_drop(player, res.value());
                }
            }break;
            case '.': execute_turns = 10; break;
            }
            //for(auto e : map(player.get<Position>()).entities) { if (e.has<Solid>() && !e.has<Player>()) e.destroy(); }
        }

        bool did_action = execute_turns > 0;
        while(execute_turns) {
            systems.turn();
            execute_turns--;
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
