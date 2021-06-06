#pragma once

#include "ecs.h"


class AISystem : public ecs::System<AISystem, 1>
{
public:
    void run();
};
