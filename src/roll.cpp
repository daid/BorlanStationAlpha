#include "roll.h"
#include "random.h"

#include <iostream>
#include <cctype>

Roll::Roll(int count, int sides, int off)
: die_count(count), die_sides(sides), offset(off)
{
}

Roll::Roll(std::string_view desc)
{
    size_t n=0;
    while(n < desc.size() && std::isdigit(desc[n])) {
        die_count = die_count * 10 + (desc[n++] - '0');
    }
    if (n == desc.size()) {
        offset = die_count;
        die_count = die_sides = 0;
        return;
    }
    if (desc[n] != 'd') {
        std::cerr << "Incorrect dice roll string: " << desc << '\n';
        die_count = die_sides = offset = 0;
        return;
    }
    n++;
    while(n < desc.size() && std::isdigit(desc[n])) {
        die_sides = die_sides * 10 + (desc[n++] - '0');
    }
    if (n == desc.size()) {
        return;
    }
    if (desc[n] != '+' && desc[n] != '-') {
        std::cerr << "Incorrect dice roll string: " << desc << '\n';
        die_count = die_sides = offset = 0;
        return;
    }
    bool negative_offset = desc[n] == '-';
    n++;
    while(n < desc.size() && std::isdigit(desc[n])) {
        offset = offset * 10 + (desc[n++] - '0');
    }
    if (negative_offset) offset = -offset;
    if (n != desc.size()) {
        std::cerr << "Incorrect dice roll string: " << desc << '\n';
        die_count = die_sides = offset = 0;
        return;
    }
    die_count = std::min(1, die_count);
}

int Roll::operator()()
{
    int result = offset;
    for(int n=0; n<die_count; n++)
        result += irandom(1, die_sides);
    return std::max(1, result);
}
