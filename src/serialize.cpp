#include "serialize.h"
#include <functional>
#include <charconv>


struct RegisteredComponentBase {
    RegisteredComponentBase(const char* _name) : name(_name), next(start) { start = this; }

    virtual void create_function(ECS::Entity entity, const SerializationInfo& info) = 0;

    static RegisteredComponentBase* start;
    const char* name;
    RegisteredComponentBase* next;
};
template<typename T> struct RegisteredComponent : public RegisteredComponentBase {
    RegisteredComponent(const char* _name) : RegisteredComponentBase(_name) {}
    void create_function(ECS::Entity entity, const SerializationInfo& info) override;
};

#define REGISTER(NAME) \
    RegisteredComponent<NAME> registered_component_ ## NAME { #NAME }; \
    template<> void RegisteredComponent<NAME>::create_function(ECS::Entity entity, const SerializationInfo& info)
#define REGISTER_TAG(NAME) \
    REGISTER(NAME) { entity.set<NAME>(); }

//==========================================================================================
//==========================================================================================
//==========================================================================================

REGISTER(Health) {
    entity.set(Health{info.num(0, "current", 0), info.num(1, "maximum", 0)});
}
REGISTER(Name) {
    entity.set(Name{std::string(info.full_string)});
}
REGISTER_TAG(Item)
REGISTER_TAG(Solid)
REGISTER_TAG(Airtight)
REGISTER_TAG(BlockVision)
REGISTER_TAG(Door)
REGISTER_TAG(Player)
REGISTER(Inventory) {
    entity.set(Inventory{});
}
REGISTER(Organic) {
    entity.set(Organic{info.num(0, "oxygen_usage", 0.02)});
}
REGISTER(Enemy) {
    entity.set(Enemy{});
}
REGISTER(Light) {
    entity.set(Light{info.num(0, "distance", 1), info.color(1, "color")});
}
REGISTER(Visual) {
    entity.set(Visual{info.chr(0, "c", '?'), info.color(1, "color"), info.num(2, "priority", 0)});
}
REGISTER(MeleeAttack) {
    entity.set(MeleeAttack{info.num(0, "accuracy", 0), info.str(1, "damage", "0")});
}
REGISTER(DamageReduction) {
    entity.set(DamageReduction{info.num(0, "amount", 0)});
}

//==========================================================================================
//==========================================================================================
//==========================================================================================

RegisteredComponentBase* RegisteredComponentBase::start;

bool deserialize_component(ECS::Entity entity, const std::string_view component, const SerializationInfo& info)
{
    for(RegisteredComponentBase* rc = RegisteredComponentBase::start; rc; rc = rc->next) {
        if (component == rc->name) {
            rc->create_function(entity, info);
            return true;
        }
    }
    return false;
}

int SerializationInfo::num(size_t index, std::string_view key, int def) const
{
    std::string_view s;
    if (kwargs.find(key) != kwargs.end()) s = kwargs.find(key)->second;
    if (args.size() > index) s = args[index];
    if (!s.empty()) {
        int result = def;
        std::from_chars(s.data(), s.data() + s.size(), result);
        return result;
    }
    return def;
}

std::string_view SerializationInfo::str(size_t index, std::string_view key, const std::string_view& def) const
{
    if (kwargs.find(key) != kwargs.end()) return kwargs.find(key)->second;
    if (args.size() > index) return args[index];
    return def;
}

HsvColor SerializationInfo::color(size_t index, std::string_view key) const
{
    std::string_view s;
    if (kwargs.find(key) != kwargs.end()) s = kwargs.find(key)->second;
    if (args.size() > index) s = args[index];
    if (!s.empty()) {
        int hue = 0, saturation = 0, value = 100;
        auto begin = std::from_chars(s.data(), s.data() + s.size(), hue).ptr;
        while(*begin == ' ') begin++;
        begin = std::from_chars(begin, s.data() + s.size(), saturation).ptr;
        while(*begin == ' ') begin++;
        std::from_chars(begin, s.data() + s.size(), value);
        return HsvColor(hue, saturation, value);
    }
    return HsvColor(0, 0, 100);
}

char SerializationInfo::chr(size_t index, std::string_view key, char def) const
{
    if (kwargs.find(key) != kwargs.end()) return kwargs.find(key)->second[0];
    if (args.size() > index) return args[index][0];
    return def;
}
