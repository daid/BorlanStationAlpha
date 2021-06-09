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

struct Name : public std::string {};

struct Position : public Vector2i
{
    void on_add(ecs::EntityBase e);
    void on_remove(ecs::EntityBase e);
};
struct VacuumFloating : public Vector2i {
    int delay{10};
};

struct Solid {};
struct Airtight{};
struct BlockVision{};
struct Door{};
struct OpenDoor{};
struct Light
{
    float distance;
    Color color{1,1,1};
};

struct Player {};

struct Visual
{
    char c;
    HsvColor color;
    int priority{0};
};

struct Item {};

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

struct Enemy {
    enum State {
        Wander,
        Attack,
    } state = Wander;

    int delay_turns{0};
};

struct MeleeAttack
{
    int accuracy;
    Roll damage;
};

struct DamageReduction {
    int amount;
};

struct Organic {
    float oxygen_usage;

    int suffocate_count{0};
};

using ECS = ecs::Engine<
    Position, VacuumFloating,
    Health, Name, Visual,
    Solid, Airtight, BlockVision, Light, Door, OpenDoor,
    Player,
    Item, Inventory, InInventory,
    Enemy, MeleeAttack, DamageReduction,
    Organic
>;
extern ECS engine;
