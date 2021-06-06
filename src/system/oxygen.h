#pragma once

#include "ecs.h"
#include "array2.h"


class OxygenSystem : public ecs::System<OxygenSystem, 10>
{
public:
    void run();

private:
    void oxygen_flow(float factor);

    Array2<int> airtight;
};
