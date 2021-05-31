#pragma once

#include "typeid.h"
#include "entitybase.h"


namespace ecs::detail {

template<typename T> struct TypeSink { using Type = void; };

template<typename T, typename=void, typename=void> struct HasEntityCallback : std::false_type{};
template<typename T> struct HasEntityCallback<T,
    typename TypeSink<decltype(&T::onCreate)>::Type,
    typename TypeSink<decltype(&T::onDestroy)>::Type> : std::true_type{};

template<typename T> class Storage {
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

}
