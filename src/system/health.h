#pragma once

#include "components.h"


class HealthSystem : public ecs::System<HealthSystem, 1>
{
public:
    void run();

    static int takeDamage(ECS::Entity e, int amount);
};
