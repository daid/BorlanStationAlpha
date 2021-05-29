#pragma once

#include "vector2.h"
#include "color.h"
#include "ecs.h"

class Health
{
public:
    int current;
    int max;
};

class Position : public Vector2i
{
public:
    void onCreate(EntityBase e);
    void onDestroy(EntityBase e);
};

class Solid {};
class BlockVision {};

class Player
{
public:
};

class Visual
{
public:
    char c;
    HsvColor color;
};

using ECS = EntityComponentSystem<
    Position, Health, Visual,
    Solid, BlockVision,
    Player
>;
extern ECS ecs;
