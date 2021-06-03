#pragma once

#include "entitybase.h"


namespace ecs {
template<typename... CL> class Entity : public EntityBase
{
public:
    template<typename T>
    bool has() const { return engine->template has<T>(*this); }

    template<typename T, typename = std::enable_if_t<!std::is_empty_v<T>>>
    T& get() const { return engine->template get<T>(*this); }

    template<typename T, typename = std::enable_if_t<!std::is_empty_v<T>>>
    Entity& set(const T& value) { engine->set(*this, value); return *this; }

    template<typename T, typename = std::enable_if_t<std::is_empty_v<T>>>
    Entity& set() { engine->template set<T>(*this); return *this; }

    template<typename T>
    Entity& remove() { engine->template remove<T>(*this); return *this; }

    void destroy() { engine->destroy(*this); }
private:
    Entity(detail::IdType _id, Engine<CL...>* _engine) : EntityBase(_id), engine(_engine) {}

    Engine<CL...>* engine;

    template<typename...> friend class Engine;
};

}