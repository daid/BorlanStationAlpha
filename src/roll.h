#pragma once

#include <string_view>


class Roll
{
public:
    Roll(int count, int sides, int off);
    // parse "XdY+Z" strings into dice rolls
    Roll(std::string_view desc);

    int operator()();

    int die_count{0};
    int die_sides{0};
    int offset{0};
};
