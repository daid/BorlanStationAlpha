#ifndef FRONTEND_H
#define FRONTEND_H

#include <vector>
#include <cstdint>

#include "array2.h"
#include "color.h"

#define INPUT_QUIT  0xF0000000
#define INPUT_RIGHT 0x10000001
#define INPUT_LEFT  0x10000002
#define INPUT_DOWN  0x10000003
#define INPUT_UP    0x10000004

class Frontend
{
public:
    Frontend();

    uint32_t getInput();

    Vector2i beginDrawing();
    void draw(int x, int y, char c, Color forground_color);
    void setbg(int x, int y, Color background_color);
    void draw(int x, int y, char c, Color forground_color, Color background_color);
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
