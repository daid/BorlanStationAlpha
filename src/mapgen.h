#ifndef MAPGEN_H
#define MAPGEN_H

#include "map.h"

class Mapgen
{
public:
    Mapgen();

private:
    bool addRoom(Vector2i position, Vector2i size);

    struct RoomInfo
    {
        Vector2i position;
        Vector2i size;
    };

    std::vector<RoomInfo> rooms;
};

#endif//MAPGEN_H
