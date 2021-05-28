#pragma once

#include <cassert>
#include <cstdlib>
#include <limits>
#include <memory>
#include <type_traits>
#include <unordered_map>


template<typename T> struct TypeSink { using Type = void; };
template<typename T, typename=void, typename=void> struct HasEntityCallback : std::false_type{};
template<typename T> struct HasEntityCallback<T,
    typename TypeSink<decltype(&T::onCreate)>::Type,
    typename TypeSink<decltype(&T::onDestroy)>::Type> : std::true_type{};


class EntityBase
{
public:
    using IdType = std::size_t;
    static constexpr IdType no_entity = std::numeric_limits<IdType>::max();

    EntityBase() : id(no_entity) {}

    bool operator==(const EntityBase& other) const { return id == other.id; }
    bool operator!=(const EntityBase& other) const { return id != other.id; }
    bool operator<(const EntityBase& other) const { return id < other.id; }
    explicit operator bool() const { return id != no_entity; }

    std::size_t hash() const { return std::hash<IdType>{}(id); }
protected:
    EntityBase(IdType _id) : id(_id) {}

    IdType id;

    template<typename... CL> friend class EntityComponentSystem;
};

template<typename... CL> class EntityComponentSystem
{
public:
    class Entity : public EntityBase
    {
    public:
        template<typename T> bool has() const { return ecs->has<T>(*this); }
        template<typename T> T& get() { return ecs->get<T>(*this); }
        template<typename T> Entity& set(const T& value) { ecs->set(*this, value); return *this; }
        template<typename T> Entity& remove() { ecs->remove<T>(*this); return *this; }

        void destroy() { ecs->destroy(*this); }
    private:
        Entity(IdType _id, EntityComponentSystem* _ecs) : EntityBase(_id), ecs(_ecs) {}

        EntityComponentSystem* ecs;

        friend class EntityComponentSystem;
    };

    Entity create()
    {
        assert(next_id != EntityBase::no_entity);
        return Entity(next_id++, this);
    }

    Entity upgrade(EntityBase e)
    {
        return Entity(e.id, this);
    }
    
    template<typename T, typename... ARGS> class Query
    {
    public:
        class Iterator
        {
        public:
            Iterator& operator++() { iter++; checkSkip(); return *this; }
            bool operator!=(const Iterator& other) const { return iter != other.iter; }
            std::tuple<Entity, T&, ARGS&...> operator*()
            {
                return {Entity(iter->first, ecs), iter->second, std::get<ComponentStorage<ARGS>>(ecs->storage).get(iter->first)...};
            }
        private:
            using iter_type = typename std::unordered_map<EntityBase::IdType, T>::iterator;
            Iterator(iter_type _iter, EntityComponentSystem* _ecs) : iter(_iter), ecs(_ecs) { checkSkip(); }

            void checkSkip()
            {
                if constexpr (sizeof...(ARGS) > 0)
                {
                    if (iter == std::get<ComponentStorage<T>>(ecs->storage).data.end())
                        return;
                    if (checkSkipPartial<ARGS...>())
                    {
                        iter++;
                        checkSkip();
                    }
                }
            }

            template<typename T2, typename... ARGS2> bool checkSkipPartial()
            {
                if (!std::get<ComponentStorage<T>>(ecs->storage).has(iter->first))
                    return true;
                if constexpr (sizeof...(ARGS2) > 0)
                    return checkSkipPartial<ARGS2...>();
                return false;
            }

            iter_type iter;
            EntityComponentSystem* ecs;

            friend class Query;
        };

        Query(EntityComponentSystem* _ecs) : ecs(_ecs) {}
        
        Iterator begin() const { return {std::get<ComponentStorage<T>>(ecs->storage).data.begin(), ecs}; }
        Iterator end() const { return {std::get<ComponentStorage<T>>(ecs->storage).data.end(), ecs}; }
    private:
        EntityComponentSystem* ecs;
    };

    template<typename T> bool has(EntityBase& e) const {  return std::get<ComponentStorage<T>>(storage).has(e.id); }
    template<typename T> T& get(EntityBase& e) { return std::get<ComponentStorage<T>>(storage).get(e.id); }
    template<typename T> void set(EntityBase& e, const T& value) { std::get<ComponentStorage<T>>(storage).set(e.id, value); }
    template<typename T> void remove(EntityBase& e) { std::get<ComponentStorage<T>>(storage).remove(e.id); }
    void destroy(EntityBase& e)
    {
        (std::get<ComponentStorage<CL>>(storage).remove(e.id), ...);
    }

    template<typename... ARGS> Query<ARGS...> query() { return Query<ARGS...>(this); }
private:
    using IdType = EntityBase::IdType;

    template<typename T> class ComponentStorage {
    public:
        std::unordered_map<IdType, T> data;

        bool has(IdType id) const { return data.find(id) != data.end(); }
        T& get(IdType id) { return data.find(id)->second; }
        void set(IdType id, const T& value)
        {
            auto res = data.insert_or_assign(id, value);
            if (res.second)
            {
                if constexpr (HasEntityCallback<T>::value)
                    res.first->second.onCreate(EntityBase(id));
            }
        }
        void remove(IdType id)
        {
            auto it = data.find(id);
            if (it != data.end())
            {
                if constexpr (HasEntityCallback<T>::value)
                    it->second.onDestroy(EntityBase(id));
                data.erase(it);
            }
        }
    };

    IdType next_id{};
    std::tuple<ComponentStorage<CL>...> storage;

    friend class Entity;
};

namespace std {
    template<> struct hash<EntityBase> { std::size_t operator()(EntityBase const& e) const noexcept { return e.hash(); } };
}
