#pragma once

#include "vector2.h"
#include "color.h"
#include "ecs.h"
#include "roll.h"

#include <vector>

enum class AmmoType {
    Bullet,
};

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
struct SingleShotVisualEffect{};

struct Item {}; //Entity can be picked up and carried around in inventory
struct Suit {}; //Entity can be equiped as a suit/armor
struct Wearing : public ecs::EntityBase {}; // Which entity is equiped as a suit
struct WornBy : public ecs::EntityBase { // We are equiped by a specific entity
    void on_add(ecs::EntityBase e);
    void on_remove(ecs::EntityBase e);
};
struct Weapon{};
struct Wielding : public ecs::EntityBase {}; // Which entity we have in hand
struct WieldBy : public ecs::EntityBase {  // This entity is wielded by a specific entity
    void on_add(ecs::EntityBase e);
    void on_remove(ecs::EntityBase e);
};
struct Ammo{
    int amount;
    int max;
    AmmoType type;
};
struct OxygenStorage {
    float current;
    float max;
};
struct Inventory {
    std::vector<ecs::EntityBase> contents;
};
struct InInventory : public ecs::EntityBase {
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

struct MeleeAttack {
    int accuracy;
    Roll damage;

    int turns{10}; // now many turns does this attack use
};
struct RangedAttack {
    int accuracy;
    Roll damage;
    int accuracy_range; //for each X cells: accuracy-1

    int turns{10}; // now many turns does this attack use
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
    Health, Name, Visual, SingleShotVisualEffect,
    Solid, Airtight, BlockVision, Light, Door, OpenDoor,
    Player,
    Item, Suit, Wearing, WornBy, Weapon, Wielding, WieldBy, Ammo, OxygenStorage,
    Inventory, InInventory,
    Enemy, MeleeAttack, RangedAttack, DamageReduction,
    Organic
>;
extern ECS engine;
