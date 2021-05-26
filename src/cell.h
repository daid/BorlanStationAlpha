#ifndef CELL_H
#define CELL_H

#include "frontend/frontend.h"

class Cell
{
public:
    enum Type
    {
        Vacuum,
        Floor,
        Wall,
        DoorH,
        DoorV,
        WindowH,
        WindowV,
    } type{Vacuum};

    void draw(Frontend& frontend, int x, int y);
};

#endif//CELL_H
