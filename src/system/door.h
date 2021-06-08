#pragma once

#include "components.h"


class DoorSystem : public ecs::System<DoorSystem, 10>
{
public:
    void run();

    static bool open(ECS::Entity door);
    static bool close(ECS::Entity door);
};
