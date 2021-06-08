#pragma once

#include "typeid.h"
#include "entitybase.h"

#include <type_traits>
#include <unordered_set>
#include <unordered_map>
#include <vector>


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
    std::vector<std::tuple<IdType, std::optional<T>>> transaction;

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

    void prepare_set(IdType id, const T& value)
    {
        transaction.push_back({id, value});
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

    void prepare_remove(IdType id)
    {
        transaction.push_back({id, {}});
    }

    void copy(IdType source, IdType target)
    {
        if (has(source)) set(target, get(source));
    }

    void prepare_copy(IdType source, IdType target)
    {
        if (has(source)) prepare_set(target, get(source));
    }

    void commit()
    {
        for(auto&& [id, value] : transaction)
            if (value.has_value())
                set(id, value.value());
            else
                remove(id);
        transaction.clear();
    }
};

template<typename T>
class Storage<T, typename std::enable_if_t<std::is_empty_v<T>>> {
public:
    using StorageType = std::unordered_set<IdType>;
    StorageType data;
    std::vector<std::tuple<IdType, bool>> transaction;

    bool has(IdType id) const
    {
        return data.find(id) != data.end();
    }

    void set(IdType id)
    {
        data.insert(id);
    }

    void prepare_set(IdType id)
    {
        transaction.push_back({id, true});
    }

    void remove(IdType id)
    {
        auto it = data.find(id);
        if (it != data.end()) {
            data.erase(it);
        }
    }

    void prepare_remove(IdType id)
    {
        transaction.push_back({id, false});
    }

    void copy(IdType source, IdType target)
    {
        if (has(source)) set(target);
    }

    void prepare_copy(IdType source, IdType target)
    {
        if (has(source)) prepare_set(target);
    }

    void commit()
    {
        for(auto&& [id, value] : transaction)
            if (value)
                set(id);
            else
                remove(id);
        transaction.clear();
    }
};

}
