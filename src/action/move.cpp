#include "move.h"
#include "map.h"
#include "messagelog.h"
#include "attack.h"
#include "system/door.h"
#include "system/health.h"


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
    int result = action_melee_attack(e, position);
    if (result)
        return result;

    if (e.has<Solid>() && map.has_entity_with<Solid>(position)) {
        return action_open_door(position);
    }
    if (e.has<VacuumFloating>()) {
        if (e.has<Player>())
            mlog.add("You try to move but are floating in space.");
        return 10;
    }
    e.remove<Position>();
    e.set(Position{position});

    if (!map(position).floor)
        e.set(VacuumFloating{offset});

    return 10;
}

void mergeAmmo(Inventory& inv, ECS::Entity ammo_entity)
{
    auto& ammo = ammo_entity.get<Ammo>();
    for(auto e : engine.upgrade(inv.contents)) {
        if (e.has<Ammo>() && !e.has<Weapon>() && e.get<Ammo>().type == ammo.type) {
            auto& other = e.get<Ammo>();
            int add = std::min(ammo.amount, other.max - other.amount);
            other.amount += add;
            ammo.amount -= add;
        }
    }
    if (ammo.amount == 0)
        ammo_entity.destroy();
}

int action_pickup(ECS::Entity e)
{
    for(auto ce : map(e.get<Position>()).entities) {
        if (ce.has<Item>()) {
            ce.remove<Position>();
            if (ce.has<Ammo>()) {
                mergeAmmo(e.get<Inventory>(), ce);
                if (!ce.has<Ammo>())
                    return 5;
            }
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

int action_equip(ECS::Entity target, ECS::Entity item)
{
    if (item.has<Suit>()) {
        if (target.has<Wearing>()) {
            auto old_item = engine.upgrade(target.get<Wearing>());
            old_item.remove<WornBy>();
            old_item.set(InInventory{target});
        }
        item.remove<InInventory>();
        item.set(WornBy{target});
        mlog.add("Put on @", item.get<Name>());
        return 10;
    }
    if (item.has<Weapon>()) {
        if (target.has<Wielding>()) {
            auto old_item = engine.upgrade(target.get<Wielding>());
            old_item.remove<WieldBy>();
            old_item.set(InInventory{target});
        }
        item.remove<InInventory>();
        item.set(WieldBy{target});
        mlog.add("You equip the @", item.get<Name>());
        return 10;
    }
    mlog.add("Cannot equip @", item.get<Name>());
    return 0;
}
