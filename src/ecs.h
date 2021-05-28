#pragma once

#include <unordered_map>
#include <memory>
#include <cstdlib>
#include <limits>


using EntityIdType = size_t;
class Entity;

template<typename... CL> class EntityComponentSystem
{
public:
    class Entity
    {
    public:
        Entity() : id(no_entity) {}

        bool operator==(const Entity& other) { return id == other.id; }
        bool operator!=(const Entity& other) { return id != other.id; }
        explicit operator bool() { return ecs; }

        template<typename T> bool has() { return ecs->has<T>(id); }
        template<typename T> T& get() { return ecs->get<T>(id); }
        template<typename T> Entity& set(const T& value) { ecs->set(id, value); return *this; }
        template<typename T> Entity& remove() { ecs->remove<T>(id); return *this; }

        void destroy() { ecs->destroy(id); }

    private:
        Entity(EntityIdType _id, EntityComponentSystem* _ecs) : id(_id), ecs(_ecs) {}
        static constexpr EntityIdType no_entity = std::numeric_limits<EntityIdType>::max();

        EntityIdType id;
        EntityComponentSystem* ecs;

        friend class EntityComponentSystem;
    };

    Entity create()
    {
        assert(next_id != Entity::no_entity);
        return Entity(next_id++, this);
    }
    
    template<typename T> class Query
    {
    public:
        class Iterator
        {
        public:
            Iterator& operator++() { iter++; return *this; }
            bool operator!=(const Iterator& other) const { return iter != other.iter; }
            std::pair<Entity, T&> operator*() { return {Entity(iter->first, ecs), iter->second}; }
        private:
            using iter_type = typename std::unordered_map<EntityIdType, T>::iterator;
            Iterator(iter_type _iter, EntityComponentSystem* _ecs) : iter(_iter), ecs(_ecs) {}

            iter_type iter;
            EntityComponentSystem* ecs;

            friend class Query;
        };

        Query(EntityComponentSystem* _ecs) : ecs(_ecs) {}
        
        Iterator begin() { return {std::get<ComponentStorage<T>>(ecs->storage).data.begin(), ecs}; }
        Iterator end() { return {std::get<ComponentStorage<T>>(ecs->storage).data.end(), ecs}; }
    private:
        EntityComponentSystem* ecs;
    };

    template<typename T> Query<T> query() { return Query<T>(this); }
private:
    template<typename T> bool has(EntityIdType id) {  return std::get<ComponentStorage<T>>(storage).has(id); }
    template<typename T> T& get(EntityIdType id) { return std::get<ComponentStorage<T>>(storage).get(id); }
    template<typename T> void set(EntityIdType id, const T& value) { std::get<ComponentStorage<T>>(storage).set(id, value); }
    template<typename T> void remove(EntityIdType id) { std::get<ComponentStorage<T>>(storage).remove(id); }

    void destroy(EntityIdType id)
    {
        (std::get<ComponentStorage<CL>>(storage).remove(id), ...);
    }

    template<typename T> class ComponentStorage {
    public:
        std::unordered_map<EntityIdType, T> data;

        bool has(EntityIdType id) {  return data.find(id) != data.end(); }
        T& get(EntityIdType id) { return data.find(id)->second; }
        void set(EntityIdType id, const T& value) { data[id] = value; }
        void remove(EntityIdType id) { data.erase(id); }
    };

    EntityIdType next_id{};
    std::tuple<ComponentStorage<CL>...> storage;

    friend class Entity;
};
