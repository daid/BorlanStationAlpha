#ifndef FRONTEND_H
#define FRONTEND_H

#include <vector>
#include <cstdint>

#include "array2.h"

#define INPUT_QUIT 0xF0000000

class Frontend
{
public:
    Frontend();

    uint32_t getInput();

    Vector2i beginDrawing();
    void draw(int x, int y, char c, uint32_t forground_color, uint32_t background_color);
    void present();
private:
    struct Tile
    {
        char c;
        uint32_t forground_color;
        uint32_t background_color;
    };
    int scale;
    Array2<Tile> buffer;
};

#endif//FRONTEND_H
