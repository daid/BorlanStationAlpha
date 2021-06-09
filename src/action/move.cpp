#include "move.h"
#include "map.h"
#include "messagelog.h"
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

int action_melee_attack(ECS::Entity e, Vector2i position)
{
    if (!e.has<MeleeAttack>())
        return 0;
    auto attack = e.get<MeleeAttack>();
    int accuracy = 10 + attack.accuracy;

    for(auto target : map(position).entities) {
        if ((e.has<Player>() && target.has<Enemy>()) || (e.has<Enemy>() && target.has<Player>())) {
            auto damage = HealthSystem::takeDamage(target, DamageType::Melee, attack.damage());
            auto roll = Roll(3, 6, 0)();
            if ((roll > 4 && roll > accuracy) || roll > 16)
                damage = 0;
            if (damage == 0) {
                if (e.has<Player>())
                    mlog.add("You miss the @", target.get<Name>());
                else
                    mlog.add("@ misses you", e.get<Name>());
            } else {
                if (e.has<Player>())
                    mlog.add("You attack @ for @ damage", target.get<Name>(), damage);
                else
                    mlog.add("@ attacks for @ damage", e.get<Name>(), damage);
            }
            return 10;
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
int action_unequip(ECS::Entity target, ECS::Entity item)
{
    return 0;
}

int action_equip(ECS::Entity target, ECS::Entity item)
{
    if (item.has<Suit>()) {
        if (target.has<Wearing>()) {
            auto old_item = engine.upgrade(target.get<Wearing>());
            old_item.remove<WornBy>();
            target.remove<Wearing>();
            old_item.set(InInventory{target});
        }
        item.remove<InInventory>();
        target.set(Wearing{item});
        item.set(WornBy{target});
        mlog.add("Put on @", item.get<Name>());
        return 10;
    }
    mlog.add("Cannot equip @", item.get<Name>());
    return 0;
}
