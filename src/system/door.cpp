#include "door.h"
#include "components.h"
#include "map.h"


void DoorSystem::run()
{
    for(auto&& [e, position] : engine.query<OpenDoor, Position>()) {
        bool can_close = true;
        map.visit_field_of_view(position, 2, [&can_close](Vector2i p) {
            for(auto ce : map(p).entities) {
                if (ce.has<Player>() || ce.has<Enemy>())
                    can_close = false;
            }
        });
        if (can_close)
            close(e);
    }
}

bool DoorSystem::open(ECS::Entity door)
{
    door.get<Visual>().c = '.';
    door.remove<Door>();
    door.set<OpenDoor>();
    door.remove<Solid>();
    door.remove<BlockVision>();
    return true;
}

bool DoorSystem::close(ECS::Entity door)
{
    door.get<Visual>().c = '+';
    door.set<Door>();
    door.remove<OpenDoor>();
    door.set<Solid>();
    door.set<BlockVision>();
    return true;
}
