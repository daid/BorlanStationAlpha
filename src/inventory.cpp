#include "components.h"

void InInventory::on_add(ecs::EntityBase e)
{
    engine.get<Inventory>(*this).contents.push_back(e);
}

void InInventory::on_remove(ecs::EntityBase e)
{
    auto& contents = engine.get<Inventory>(*this).contents;
    contents.erase(std::remove_if(contents.begin(), contents.end(), [e](const ecs::EntityBase& ce) { return e == ce; }), contents.end());
}

void WornBy::on_add(ecs::EntityBase e)
{
    engine.set(*this, Wearing{e});
}

void WornBy::on_remove(ecs::EntityBase e)
{
    engine.remove<Wearing>(*this);
}

void WieldBy::on_add(ecs::EntityBase e)
{
    engine.set(*this, Wielding{e});
}

void WieldBy::on_remove(ecs::EntityBase e)
{
    engine.remove<Wielding>(*this);
}
