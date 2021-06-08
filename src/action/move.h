#pragma once

#include "components.h"


int action_open_door(Vector2i position);
int action_move(ECS::Entity e, Vector2i offset);
int action_pickup(ECS::Entity e);
int action_drop(ECS::Entity from, ECS::Entity target);
