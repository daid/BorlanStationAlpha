#pragma once

#include "typeid.h"
#include "entitybase.h"

#include <type_traits>
#include <unordered_set>
#include <unordered_map>


namespace ecs::detail {

template<typename T> struct TypeSink { using Type = void; };

template<typename T, typename=void, typename=void> struct HasEntityCallback : std::false_type{};
template<typename T> struct HasEntityCallback<T,
    typename TypeSink<decltype(&T::on_add)>::Type,
    typename TypeSink<decltype(&T::on_remove)>::Type> : std::true_type{};

template<typename T>
class Storage<T, typename std::enable_if_t<!std::is_empty_v<T>>> {
public:
    using StorageType = std::unordered_map<IdType, T>;
    StorageType data;

    bool has(IdType id) const {
        return data.find(id) != data.end();
    }

    T& get(IdType id) {
        return data.find(id)->second;
    }

    void set(IdType id, const T& value)
    {
        auto res = data.insert_or_assign(id, value);
        if (res.second) {
            if constexpr (HasEntityCallback<T>::value)
                res.first->second.on_add(EntityBase(id));
        }
    }

    void remove(IdType id)
    {
        auto it = data.find(id);
        if (it != data.end()) {
            if constexpr (HasEntityCallback<T>::value)
                it->second.on_remove(EntityBase(id));
            data.erase(it);
        }
    }
};

template<typename T>
class Storage<T, typename std::enable_if_t<std::is_empty_v<T>>> {
public:
    using StorageType = std::unordered_set<IdType>;
    StorageType data;

    bool has(IdType id) const {
        return data.find(id) != data.end();
    }

    void set(IdType id)
    {
        data.insert(id);
    }

    void remove(IdType id)
    {
        auto it = data.find(id);
        if (it != data.end()) {
            data.erase(it);
        }
    }
};

}
