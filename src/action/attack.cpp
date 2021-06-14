#include "move.h"
#include "map.h"
#include "messagelog.h"
#include "traceline.h"
#include "system/door.h"
#include "system/health.h"


static bool accuracy_roll(int accuracy) {
    auto roll = Roll(3, 6, 0)();
    if (roll < 5)
        return true;
    if (roll > 16)
        return false;
    if (roll > accuracy)
        return false;
    return true;
}

int action_melee_attack(ECS::Entity e, Vector2i position)
{
    MeleeAttack attack{-10, {0, 0, 0}};
    if (e.has<MeleeAttack>())
        attack = e.get<MeleeAttack>();
    if (e.has<Wielding>()) {
        auto weapon = engine.upgrade(e.get<Wielding>());
        if (weapon.has<MeleeAttack>())
            attack = weapon.get<MeleeAttack>();
    }
    if (attack.accuracy == -10)
        return 0;
    int accuracy = 10 + attack.accuracy;

    for(auto target : map(position).entities) {
        if ((e.has<Player>() && target.has<Enemy>()) || (e.has<Enemy>() && target.has<Player>())) {
            auto damage = HealthSystem::takeDamage(target, DamageType::Melee, attack.damage());
            if (accuracy_roll(accuracy))
                damage = 0;
            if (damage == 0) {
                if (e.has<Player>())
                    mlog.add("You miss the @", target.get<Name>());
                else
                    mlog.add("@ misses you", e.get<Name>());
            } else {
                if (e.has<Player>())
                    mlog.add("You attack the @ for @ damage", target.get<Name>(), damage);
                else
                    mlog.add("@ attacks for @ damage", e.get<Name>(), damage);
            }
            return attack.turns;
        }
    }
    return 0;
}

int action_ranged_attack(ECS::Entity owner, Vector2i position)
{
    RangedAttack attack{-10, {0, 0, 0}, 0};
    if (owner.has<RangedAttack>())
        attack = owner.get<RangedAttack>();
    if (owner.has<Wielding>()) {
        auto weapon = engine.upgrade(owner.get<Wielding>());
        if (weapon.has<RangedAttack>())
            attack = weapon.get<RangedAttack>();
        if (weapon.has<Ammo>()) {
            auto& ammo = weapon.get<Ammo>();
            if (ammo.amount == 0) {
                if (owner.has<Player>())
                    mlog.add("Your @ is empty!", weapon.get<Name>());
                return 0;
            }
            ammo.amount -= 1;
        }
    }
    if (attack.accuracy == -10)
        return 0;
    int accuracy = 10 + attack.accuracy;
    auto start = owner.get<Position>();
    auto shot_vector = position - start;
    auto final_target = start + shot_vector * 100;
    auto damage = attack.damage();
    char trace_visual = '?';
    if (std::abs(shot_vector.x) <= std::abs(shot_vector.y) / 2)
        trace_visual = '|';
    else if (std::abs(shot_vector.y) <= std::abs(shot_vector.x) / 2)
        trace_visual = '-';
    else if (std::signbit(shot_vector.x) == std::signbit(shot_vector.y))
        trace_visual = '\\';
    else
        trace_visual = '/';
    for(auto p : TraceLine(start, final_target)) {
        if (!map.inside(p)) break;
        if (p == start) continue;
        int distance = (start - p).length();
        for(auto e : map(p).entities) {
            if (e.has<Solid>() && accuracy_roll(accuracy - distance / attack.accuracy_range)) {
                if (auto result = HealthSystem::takeDamage(e, DamageType::Bullet, damage)) {
                    if (owner.has<Player>())
                        mlog.add("You hit the @ for @ damage", e.get<Name>(), result);
                    else
                        mlog.add("@ hits you for @ damage", owner.get<Name>(), result);
                    return attack.turns;
                }
            }
            if (e.has<Airtight>())
                return attack.turns;
        }
        engine.create().set(Position{p}).set(Visual{trace_visual, HsvColor(0, 0, 75), -9}).set<SingleShotVisualEffect>();
    }
    return attack.turns;
}
