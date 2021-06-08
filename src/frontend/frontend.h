#ifndef FRONTEND_H
#define FRONTEND_H

#include <vector>
#include <cstdint>
#include <string_view>

#include "array2.h"
#include "color.h"

#define INPUT_QUIT        0xF0000000
#define INPUT_RIGHT       0x10000001
#define INPUT_LEFT        0x10000002
#define INPUT_DOWN        0x10000003
#define INPUT_UP          0x10000004
#define INPUT_DOWN_LEFT   0x10000005
#define INPUT_UP_LEFT     0x10000006
#define INPUT_DOWN_RIGHT  0x10000007
#define INPUT_UP_RIGHT    0x10000008

#define INPUT_CANCEL 0x10000010

class Frontend
{
public:
    Frontend();
    ~Frontend();

    uint32_t get_input();

    Vector2i begin_drawing();
    Vector2i get_draw_size();
    void draw(int x, int y, char c, Color forground_color);
    void setbg(int x, int y, Color background_color);
    void draw(int x, int y, std::string_view str, Color forground_color);
    void draw(int x, int y, std::string_view str, Color forground_color, Color background_color);
    void draw(int x, int y, char c, Color forground_color, Color background_color);
    void fill(Vector2i position, Vector2i size, char c, Color forground_color, Color background_color);
    void present();
private:
    struct Tile
    {
        char c;
        Color forground_color;
        Color background_color;
    };
    int scale;
    Array2<Tile> buffer;
};

#endif//FRONTEND_H
