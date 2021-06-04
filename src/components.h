#pragma once

#include "vector2.h"
#include "color.h"
#include "ecs.h"

#include <vector>


class Health
{
public:
    int current;
    int max;
};

class Position : public Vector2i
{
public:
    void on_add(ecs::EntityBase e);
    void on_remove(ecs::EntityBase e);
};

class Solid {};
class BlockVision {};
class Light {
public:
    float distance;
    Color color{1,1,1};
};

class Player
{
public:
};

class Visual
{
public:
    char c;
    HsvColor color;
    int priority{0};
};

class Item
{
public:
    std::string name;
};

class Inventory
{
public:
    std::vector<ecs::EntityBase> contents;
};

using ECS = ecs::Engine<
    Position, Health, Visual,
    Solid, BlockVision, Light,
    Player,
    Item, Inventory
>;
extern ECS engine;
