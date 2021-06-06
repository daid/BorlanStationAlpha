#pragma once

#include "ecs.h"


class VisionSystem : public ecs::System<VisionSystem, 1>
{
public:
    void run();
};
