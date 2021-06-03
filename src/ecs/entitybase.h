#pragma once

#include "typeid.h"


namespace ecs::detail {
    template<typename T, typename Enable = void> class Storage;
}
namespace ecs {
template<typename...> class Engine;

class EntityBase
{
public:
    EntityBase() : id(detail::NO_ENTITY) {}

    bool operator==(const EntityBase& other) const { return id == other.id; }
    bool operator!=(const EntityBase& other) const { return id != other.id; }
    bool operator<(const EntityBase& other) const { return id < other.id; }
    explicit operator bool() const { return id != detail::NO_ENTITY; }

    std::size_t hash() const { return std::hash<detail::IdType>{}(id); }
protected:
    EntityBase(detail::IdType _id) : id(_id) {}

    detail::IdType id;

    template<typename...> friend class ecs::Engine;
    template<typename, typename> friend class ecs::detail::Storage;
};

}

namespace std {
    template<> struct hash<ecs::EntityBase> { std::size_t operator()(ecs::EntityBase const& e) const noexcept { return e.hash(); } };
}
