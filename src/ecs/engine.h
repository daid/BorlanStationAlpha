#pragma once

#include <tuple>
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

    Entity create()
    {
        assert(next_id != ecs::detail::no_entity);
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
            using ResultType = typename Cons<Entity, typename EmptyFilter<T&, ARGS&...>::type>::type;

            Iterator& operator++() { iter++; checkSkip(); return *this; }
            bool operator!=(const Iterator& other) const { return iter != other.iter; }
            ResultType operator*()
            {
                if constexpr (std::is_empty_v<T>)
                    return std::tuple_cat(std::tuple<Entity>(Entity(iter->first, engine)), get<ARGS>()...);
                else
                    return std::tuple_cat(std::tuple<Entity, T&>(Entity(iter->first, engine), iter->second), get<ARGS>()...);
            }

        private:
            template<typename T2, typename = std::enable_if_t<std::is_empty_v<T2>>> std::tuple<> get() { return {}; }
            template<typename T2, typename = std::enable_if_t<!std::is_empty_v<T2>>> std::tuple<T2&> get()
            {
                return {std::get<ecs::detail::Storage<T2>>(engine->storage).get(iter->first)};
            }

            using iter_type = typename ecs::detail::Storage<T>::StorageType::iterator;
            Iterator(iter_type _iter, Engine* _engine) : iter(_iter), engine(_engine) { checkSkip(); }

            void checkSkip()
            {
                if constexpr (sizeof...(ARGS) > 0)
                {
                    if (iter == std::get<ecs::detail::Storage<T>>(engine->storage).data.end())
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
                if (!std::get<ecs::detail::Storage<T2>>(engine->storage).has(iter->first))
                    return true;
                if constexpr (sizeof...(ARGS2) > 0)
                    return checkSkipPartial<ARGS2...>();
                return false;
            }

            iter_type iter;
            Engine* engine;

            friend class Query;
        };

        Query(Engine* _engine) : engine(_engine) {}
        
        Iterator begin() const { return {std::get<ecs::detail::Storage<T>>(engine->storage).data.begin(), engine}; }
        Iterator end() const { return {std::get<ecs::detail::Storage<T>>(engine->storage).data.end(), engine}; }
    private:
        Engine* engine;
    };

    template<typename T> bool has(const ecs::EntityBase& e) const {  return std::get<ecs::detail::Storage<T>>(storage).has(e.id); }
    template<typename T> T& get(const ecs::EntityBase& e) { return std::get<ecs::detail::Storage<T>>(storage).get(e.id); }
    template<typename T> void set(const ecs::EntityBase& e, const T& value) { std::get<ecs::detail::Storage<T>>(storage).set(e.id, value); }
    template<typename T> void remove(const ecs::EntityBase& e) { std::get<ecs::detail::Storage<T>>(storage).remove(e.id); }
    void destroy(ecs::EntityBase& e)
    {
        (std::get<ecs::detail::Storage<CL>>(storage).remove(e.id), ...);
    }

    template<typename... ARGS> Query<ARGS...> query() { return Query<ARGS...>(this); }
private:
    using IdType = ecs::detail::IdType;

    IdType next_id{};
    std::tuple<ecs::detail::Storage<CL>...> storage;
};

}
