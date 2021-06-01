#ifndef CELL_H
#define CELL_H

#include "frontend/frontend.h"
#include "components.h"

#include <set>


class Cell
{
public:
    bool floor{false};
    bool visible{false};
    Color light_level{};
    float oxygen{0};

    char last_seen_as{' '};

    std::set<ECS::Entity> entities;
};

#endif//CELL_H
