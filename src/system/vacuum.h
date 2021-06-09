#pragma once

#include "ecs.h"


class VacuumSystem : public ecs::System<VacuumSystem, 1>
{
public:
    void run();
};
