#include "cell.h"

void Cell::draw(Frontend& frontend, int x, int y)
{
    switch(type)
    {
    case Vacuum:    frontend.draw(x, y, ' ', Color(255,255,255), Color(0x20,0x20,0x20)); break;
    /*case Floor:     frontend.draw(x, y, '.', 0xFFFFFF, 0x202020); break;
    case Wall:      frontend.draw(x, y, '#', 0xFFFFFF, 0x202020); break;
    case DoorH:     frontend.draw(x, y, '-', 0xFFFFFF, 0x202020); break;
    case DoorV:     frontend.draw(x, y, '|', 0xFFFFFF, 0x202020); break;
    case WindowH:   frontend.draw(x, y, '-', 0x8080FF, 0x202020); break;
    case WindowV:   frontend.draw(x, y, '|', 0x8080FF, 0x202020); break;*/
    }
}
