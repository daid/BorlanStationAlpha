#include "map.h"
#include <cstdio>

Array2<Cell> map;

void Position::onCreate(EntityBase e)
{
    map(x, y).entities.insert(ecs.upgrade(e));
    printf("C: %d %d\n", x, y);
}

void Position::onDestroy(EntityBase e)
{
    map(x, y).entities.erase(ecs.upgrade(e));
    printf("D: %d %d\n", x, y);
}
