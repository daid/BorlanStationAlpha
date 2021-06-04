#include "frontend.h"


Vector2i Frontend::get_draw_size()
{
    return buffer.size();
}

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

void Frontend::draw(int x, int y, std::string_view str, Color forground_color)
{
    for(auto c : str)
        draw(x++, y, c, forground_color);
}

void Frontend::draw(int x, int y, std::string_view str, Color forground_color, Color background_color)
{
    for(auto c : str)
        draw(x++, y, c, forground_color, background_color);
}

void Frontend::draw(int x, int y, char c, Color forground_color, Color background_color)
{
    if (x < 0 || y < 0) return;
    if (x >= buffer.size().x || y >= buffer.size().y) return;

    buffer(x, y) = Tile{c, forground_color, background_color};
}

void Frontend::fill(Vector2i position, Vector2i size, char c, Color forground_color, Color background_color)
{
    for(int y=0; y<size.y; y++) {
        for(int x=0; x<size.x; x++) {
            draw(position.x + x, position.y + y, c, forground_color, background_color);
        }
    }
}