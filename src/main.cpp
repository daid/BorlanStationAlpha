#include <cstdio>
#include <cassert>
#include "frontend/frontend.h"
#include "cell.h"
#include "map.h"
#include "ecs.h"
#include "mapgen.h"
#include "components.h"
#include "messagelog.h"
#include "blueprint.h"

#include "system/vision.h"
#include "system/oxygen.h"
#include "system/ai.h"
#include "system/door.h"
#include "system/health.h"
#include "system/vacuum.h"

#include "view/map.h"
#include "view/hud.h"
#include "view/menu.h"

#include "action/move.h"

ECS engine;
ecs::Systems<
    AISystem,
    DoorSystem,
    OxygenSystem,
    HealthSystem,
    VacuumSystem,
    VisionSystem
> systems;


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
    load_blueprints();

    Mapgen mg;

    auto player = create_blueprint("Player").set(Position{mg.start});

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
        if (input == INPUT_QUIT) return 0;
        int execute_turns = 0;
        if (!mlog.has_new_messages() && player.has<Position>()) {
            switch(input) {
            case INPUT_RIGHT: execute_turns = action_move(player, Vector2i{1, 0}); break;
            case INPUT_LEFT: execute_turns = action_move(player, Vector2i{-1, 0}); break;
            case INPUT_DOWN: execute_turns = action_move(player, Vector2i{0, 1}); break;
            case INPUT_UP: execute_turns = action_move(player, Vector2i{0, -1}); break;
            case INPUT_DOWN_LEFT: execute_turns = action_move(player, Vector2i{-1, 1}); break;
            case INPUT_UP_LEFT: execute_turns = action_move(player, Vector2i{-1, -1}); break;
            case INPUT_DOWN_RIGHT: execute_turns = action_move(player, Vector2i{1, 1}); break;
            case INPUT_UP_RIGHT: execute_turns = action_move(player, Vector2i{1, -1}); break;
            case 'p': execute_turns = action_pickup(player); break;
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
