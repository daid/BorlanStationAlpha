#pragma once

#include <tuple>
#include <vector>
#include <type_traits>


namespace ecs {

template <typename, typename> struct Cons;
template <typename  T, typename ...Args> struct Cons<T, std::tuple<Args...>> { using type = std::tuple<T, Args...>; };

template<typename...> struct EmptyFilter;
template<> struct EmptyFilter<> { using type = std::tuple<>; };
template<typename T, typename... ARGS> struct EmptyFilter<T, ARGS...> {
    using type = typename std::conditional_t<
        !std::is_empty_v<std::remove_reference_t<T>>,
        typename Cons<T, typename EmptyFilter<ARGS...>::type>::type,
        typename EmptyFilter<ARGS...>::type>;
};

template<typename... CL> class Engine
{
public:
    using Entity = ecs::Entity<CL...>;
    using ComponentsTuple = std::tuple<CL...>;

    Entity create()
    {
        assert(next_id != ecs::detail::NO_ENTITY);
        return Entity(next_id++, this);
    }

    Entity copy(const EntityBase& e)
    {
        Entity result = create();
        (std::get<ecs::detail::Storage<CL>>(storage).copy(e.id, result.id), ...);
        return result;
    }

    Entity upgrade(EntityBase e)
    {
        return Entity(e.id, this);
    }

    class VectorWrap
    {
    public:
        class Iterator : public std::vector<ecs::EntityBase>::iterator {
        public:
            Iterator(Engine* _engine, const std::vector<ecs::EntityBase>::iterator& i) : std::vector<ecs::EntityBase>::iterator(i), engine(_engine) {}
            Iterator(const std::vector<ecs::EntityBase>::iterator& i) : std::vector<ecs::EntityBase>::iterator(i), engine(nullptr) {}

            Entity operator*() { return engine->upgrade(std::vector<ecs::EntityBase>::iterator::operator*()); }
        private:
            Engine* engine;
        };
        VectorWrap(Engine* _engine, std::vector<ecs::EntityBase>& _v) : engine(_engine), v(_v) {}

        Iterator begin() { return {engine, v.begin()}; }
        Iterator end() { return v.end(); }
    private:
        Engine* engine;
        std::vector<ecs::EntityBase>& v;
    };

    VectorWrap upgrade(std::vector<ecs::EntityBase>& v)
    {
        return VectorWrap(this, v);
    }

    template<typename T, typename... ARGS> class Query
    {
    public:
        class Iterator
        {
        public:
            using ResultType = typename Cons<Entity, typename EmptyFilter<T&, ARGS&...>::type>::type;

            Iterator& operator++() { iter++; check_skip(); return *this; }
            bool operator!=(const Iterator& other) const { return iter != other.iter; }
            ResultType operator*()
            {
                return std::tuple_cat(std::tuple<Entity>(Entity(*iter, engine)), get<T>(), get<ARGS>()...);
            }

        private:
            template<typename T2, typename = std::enable_if_t<std::is_empty_v<T2>>> std::tuple<> get() { return {}; }
            template<typename T2, typename = std::enable_if_t<!std::is_empty_v<T2>>> std::tuple<T2&> get() {
                return {std::get<ecs::detail::Storage<T2>>(engine->storage).get(*iter)};
            }

            using IteratorType = typename std::vector<ecs::detail::IdType>::const_iterator;
            Iterator(IteratorType _iter, IteratorType _end_iter, Engine* _engine) : iter(_iter), end_iter(_end_iter), engine(_engine) { check_skip(); }

            void check_skip()
            {
                if (iter == end_iter)
                    return;
                if (check_skip_partial<T, ARGS...>()) {
                    iter++;
                    check_skip();
                }
            }

            template<typename T2, typename... ARGS2> bool check_skip_partial()
            {
                if (!std::get<ecs::detail::Storage<T2>>(engine->storage).has(*iter))
                    return true;
                if constexpr (sizeof...(ARGS2) > 0)
                    return check_skip_partial<ARGS2...>();
                return false;
            }

            IteratorType iter, end_iter;
            Engine* engine;

            friend class Query;
        };

        Query(Engine* _engine) : engine(_engine) {
            for(auto it : std::get<ecs::detail::Storage<T>>(engine->storage).data)
                if constexpr(std::is_empty_v<T>)
                    ids.push_back(it);
                else
                    ids.push_back(it.first);
        }
        
        Iterator begin() const { return {ids.begin(), ids.end(), engine}; }
        Iterator end() const { return {ids.end(), ids.end(), engine}; }
    private:
        Engine* engine;
        std::vector<ecs::detail::IdType> ids;
    };

    template<typename T>
    bool has(const ecs::EntityBase& e) const { return std::get<ecs::detail::Storage<T>>(storage).has(e.id); }
    
    template<typename T, typename = std::enable_if_t<!std::is_empty_v<T>>>
    T& get(const ecs::EntityBase& e) { return std::get<ecs::detail::Storage<T>>(storage).get(e.id); }

    template<typename T, typename = std::enable_if_t<!std::is_empty_v<T>>>
    void set(const ecs::EntityBase& e, const T& value) {
        std::get<ecs::detail::Storage<T>>(storage).set(e.id, value);
    }

    template<typename T, typename = std::enable_if_t<std::is_empty_v<T>>>
    void set(const ecs::EntityBase& e) {
        std::get<ecs::detail::Storage<T>>(storage).set(e.id);
    }
    
    template<typename T>
    void remove(const ecs::EntityBase& e) {
        std::get<ecs::detail::Storage<T>>(storage).remove(e.id);
    }
    
    void destroy(ecs::EntityBase& e) {
        (std::get<ecs::detail::Storage<CL>>(storage).remove(e.id), ...);
    }

    template<typename... ARGS> Query<ARGS...> query() { return Query<ARGS...>(this); }
private:
    using IdType = ecs::detail::IdType;

    IdType next_id{};
    std::tuple<ecs::detail::Storage<CL>...> storage;
};

}
