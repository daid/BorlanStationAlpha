#pragma once

#include <vector>
#include <cstdlib>
#include <cassert>
#include <tuple>
#include <limits>


template<typename... CS> class EntityComponentSystem
{
public:
    class Entity
    {
    public:
        template<typename T> Entity& add(const T& value)
        {
            ecs.add(*this, value);
            return *this;
        }

        template<typename T> Entity& remove()
        {
            ecs.remove<T>(*this);
            return *this;
        }

        template<typename T> bool has()
        {
            return ecs.has<T>(*this);
        }

        template<typename T> T& get()
        {
            return ecs.get<T>(*this);
        }
    private:
        size_t id;
        EntityComponentSystem<CS...>& ecs;

        Entity(size_t _id, EntityComponentSystem<CS...>& _ecs) : id(_id), ecs(_ecs) {}

        template<typename...> friend class EntityComponentSystem;
    };

    Entity create()
    {
        auto new_id = next_entity_id++;
        assert(next_entity_id != 0);
        (std::get<ComponentStorage<CS>>(storage).entity_to_component_mapping.resize(next_entity_id, std::numeric_limits<size_t>::max()), ...);
        return Entity(new_id, *this);
    }

    template<typename C> void add(Entity e, const C& value)
    {
        std::get<ComponentStorage<C>>(storage).add(e.id, value);
    }

    template<typename C> void remove(Entity e)
    {
        std::get<ComponentStorage<C>>(storage).remove(e.id);
    }

    template<typename C> bool has(Entity e)
    {
        return std::get<ComponentStorage<C>>(storage).has(e.id);
    }

    template<typename C> C& get(Entity e)
    {
        return std::get<ComponentStorage<C>>(storage).get(e.id);
    }
private:
    template<typename C> class ComponentStorage
    {
    public:
        std::vector<std::array<char, sizeof(C)>> components;
        std::vector<size_t> component_to_entity_mapping;
        std::vector<size_t> entity_to_component_mapping;

        void add(size_t id, const C& value)
        {
            assert(entity_to_component_mapping[id] == std::numeric_limits<size_t>::max());
            entity_to_component_mapping[id] = components.size();
            components.push_back({});
            *std::launder(reinterpret_cast<C*>(&components.back())) = value;
            component_to_entity_mapping.push_back(id);
        }

        void remove(size_t id)
        {
            auto idx = entity_to_component_mapping[id];
            if (idx == std::numeric_limits<size_t>::max())
                return;
            *std::launder(reinterpret_cast<C*>(&components[idx])) = std::move(*std::launder(reinterpret_cast<C*>(&components.back())));
            component_to_entity_mapping[idx] = component_to_entity_mapping.back();
            entity_to_component_mapping[component_to_entity_mapping.back()] = idx;
            components.pop_back();
            component_to_entity_mapping.pop_back();
        }

        bool has(size_t id)
        {
            return entity_to_component_mapping[id] != std::numeric_limits<size_t>::max();
        }

        C& get(size_t id)
        {
            auto idx = entity_to_component_mapping[id];
            assert(idx != std::numeric_limits<size_t>::max());
            return *std::launder(reinterpret_cast<C*>(&components[idx]));
        }
    };
    std::tuple<ComponentStorage<CS>...> storage;

    size_t next_entity_id{0};
};
