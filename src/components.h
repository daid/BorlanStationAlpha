#pragma once

#include "vector2.h"
#include "color.h"
#include "ecs.h"
#include "roll.h"

#include <vector>


struct Health
{
    int current;
    int max;
};

struct Position : public Vector2i
{
    void on_add(ecs::EntityBase e);
    void on_remove(ecs::EntityBase e);
};

struct Solid {};
struct BlockVision {};
struct Light {
    float distance;
    Color color{1,1,1};
};

struct Player
{
};

struct Visual
{
    char c;
    HsvColor color;
    int priority{0};
};

struct Item
{
    std::string name;
};

struct Inventory
{
    std::vector<ecs::EntityBase> contents;
};
struct InInventory
{
    ecs::EntityBase container;

    void on_add(ecs::EntityBase e);
    void on_remove(ecs::EntityBase e);
};

struct Enemy {};

struct NaturalMeleeAttack
{
    int accuracy;
    Roll damage;
};

struct RandomWalk
{
    int countdown;
    int delay;
};

using ECS = ecs::Engine<
    Position, Health, Visual,
    Solid, BlockVision, Light,
    Player,
    Item, Inventory, InInventory,
    RandomWalk
>;
extern ECS engine;
