#pragma once

#include "components.h"

enum class DamageType {
    Suffocate,
    Fire,
    Melee,
    Bullet,
};

class HealthSystem : public ecs::System<HealthSystem, 1>
{
public:
    void run();

    static int takeDamage(ECS::Entity e, DamageType type, int amount);
};
