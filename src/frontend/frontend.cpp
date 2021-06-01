#include "frontend.h"


void Frontend::draw(int x, int y, char c, Color forground_color)
{
    if (x < 0 || y < 0) return;
    if (x >= buffer.size().x || y >= buffer.size().y) return;

    auto& e = buffer(x, y);
    e.c = c;
    e.forground_color = forground_color;
}

void Frontend::setbg(int x, int y, Color background_color)
{
    if (x < 0 || y < 0) return;
    if (x >= buffer.size().x || y >= buffer.size().y) return;

    auto& e = buffer(x, y);
    e.background_color = background_color;
}

void Frontend::draw(int x, int y, char c, Color forground_color, Color background_color)
{
    if (x < 0 || y < 0) return;
    if (x >= buffer.size().x || y >= buffer.size().y) return;

    buffer(x, y) = Tile{c, forground_color, background_color};
}