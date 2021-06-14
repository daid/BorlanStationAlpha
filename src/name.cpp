#include "name.h"
#include "format.h"

std::string get_name(ECS::Entity e) {
    std::string result;
    if (e.has<Name>())
        result = e.get<Name>();
    else
        result = "ERR:NO-NAME";
    
    if (e.has<Ammo>()) {
        const auto& ammo = e.get<Ammo>();
        if (e.has<Weapon>()) {
            result += format(" [@/@]", ammo.amount, ammo.max);
        } else {
            result += format(" x@", ammo.amount);
        }
    }
    if (e.has<Suit>() && e.has<Health>()) {
        const auto& health = e.get<Health>();
        result += format(" [@%]", (health.current * 100) / health.max);
    }

    return result;
}