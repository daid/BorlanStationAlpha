#include "vacuum.h"
#include "components.h"
#include "map.h"


void VacuumSystem::run()
{
    for(auto&& [e, floating, position] : engine.query<VacuumFloating, Position>()) {
        if (map.has_entity_with<Airtight>(position + Vector2i{-1, 0})
            || map.has_entity_with<Airtight>(position + Vector2i{1, 0})
            || map.has_entity_with<Airtight>(position + Vector2i{0,-1})
            || map.has_entity_with<Airtight>(position + Vector2i{0, 1})) {
            e.remove<VacuumFloating>();
        } else if (floating.delay) {
            floating.delay--;
        } else {
            if (map.has_entity_with<Solid>(position + floating)) {
                e.remove<VacuumFloating>();
            } else {
                Position new_position{position + floating};
                e.remove<Position>();
                e.set(new_position);
                floating.delay = 9;
            }
        }
    }
}
