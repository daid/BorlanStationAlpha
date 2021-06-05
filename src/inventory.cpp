#include "components.h"

void InInventory::on_add(ecs::EntityBase e)
{
    engine.get<Inventory>(container).contents.push_back(e);
}

void InInventory::on_remove(ecs::EntityBase e)
{
    auto& contents = engine.get<Inventory>(container).contents;
    contents.erase(std::remove_if(contents.begin(), contents.end(), [e](const ecs::EntityBase& ce) { return e == ce; }), contents.end());
}
