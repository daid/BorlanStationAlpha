#pragma once

#include "components.h"

struct SerializationInfo
{
    std::string_view full_string;
    std::vector<std::string_view> args;
    std::unordered_map<std::string_view, std::string_view> kwargs;

    int num(size_t index, std::string_view key, int def) const;
    float flt(size_t index, std::string_view key, float def) const;
    std::string_view str(size_t index, std::string_view key, const std::string_view& def) const;
    HsvColor color(size_t index, std::string_view key) const;
    char chr(size_t index, std::string_view key, char def) const;
};

bool deserialize_component(ECS::Entity entity, const std::string_view component, const SerializationInfo& info);
