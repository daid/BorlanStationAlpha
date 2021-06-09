#include "vacuum.h"
#include "components.h"
#include "map.h"


void VacuumSystem::run()
{
    for(auto&& [e, floating, position] : engine.query<VacuumFloating, Position>()) {
        if (map.has_solid_entity(position + Vector2i{-1, 0})
            || map.has_solid_entity(position + Vector2i{1, 0})
            || map.has_solid_entity(position + Vector2i{0, -1})
            || map.has_solid_entity(position + Vector2i{0, 1})) {
            e.remove<VacuumFloating>();
        } else if (floating.delay) {
            floating.delay--;
        } else {
            if (map.has_solid_entity(position + floating)) {
                e.remove<VacuumFloating>();
            } else {
                e.remove<Position>();
                e.set(Position{position + floating});
                floating.delay = 9;
            }
        }
    }
}
