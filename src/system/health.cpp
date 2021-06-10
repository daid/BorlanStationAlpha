#include "health.h"
#include "action/move.h"
#include "messagelog.h"
#include "map.h"


void HealthSystem::run()
{
    for(auto&& [e, health] : engine.query<Health>()) {
        if (health.current <= 0) {
            if (e.has<Player>())
                mlog.add("You died");
            if (e.has<Enemy>() && map(e.get<Position>()).visible)
                mlog.add("@ died", e.get<Name>());
            if (e.has<Inventory>()) {
                auto& inv = e.get<Inventory>();
                for(auto i : engine.upgrade(inv.contents)) {
                    action_drop(e, i);
                }
            }
            e.destroy();
        }
    }
}

int HealthSystem::takeDamage(ECS::Entity e, DamageType type, int amount)
{
    if (!e.has<Health>()) return 0;
    auto& health = e.get<Health>();

    if (type != DamageType::Suffocate) {
        if (e.has<DamageReduction>()) {
            amount = std::max(0, amount - e.get<DamageReduction>().amount);
        }
        if (e.has<Wearing>()) {
            auto suit = engine.upgrade(e.get<Wearing>());
            if (suit.has<DamageReduction>()) {
                amount = std::max(0, amount - suit.get<DamageReduction>().amount);
            }
        }
    }
    health.current = std::max(0, health.current - amount);
    return amount;
}
