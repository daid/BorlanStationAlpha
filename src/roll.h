#pragma once

#include <string_view>


class Roll
{
public:
    // parse "XdY+Z"
    Roll(std::string_view desc);

    int operator()();
private:
    int die_count{0};
    int die_sides{0};
    int offset{0};
};