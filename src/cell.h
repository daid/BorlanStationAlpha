#ifndef CELL_H
#define CELL_H

#include "frontend/frontend.h"
#include "components.h"

#include <set>

class Cell
{
public:
    bool floor{false};

    std::set<ECS::Entity> entities;
};

#endif//CELL_H
