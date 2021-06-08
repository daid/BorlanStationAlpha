#include "move.h"
#include "map.h"
#include "system/door.h"


int action_open_door(Vector2i position)
{
    for(auto ce : map(position).entities) {
        if (ce.has<Door>()) {
            if (DoorSystem::open(ce))
                return 5;
        }
    }
    return 0;
}

int action_move(ECS::Entity e, Vector2i offset)
{
    auto position = e.get<Position>() + offset;
    if (e.has<Solid>() && map.has_solid_entity(position)) {
        return action_open_door(position);
    }
    e.remove<Position>();
    e.set(Position{position});
    return 10;
}

int action_pickup(ECS::Entity e)
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
