#ifndef MAPGEN_H
#define MAPGEN_H

#include "map.h"

class Mapgen
{
public:
    Mapgen();

private:
    void setWallH(Vector2i position);
    void setWallV(Vector2i position);

    bool addRoom(Vector2i position, Vector2i size);

    bool isWallForDoor(Vector2i position);
    bool isWallForWindow(Vector2i position);

    enum SimpleCell {
        Unset,
        Vacuum,
        Floor,
        Wall,
        ThinWall,
        Door,
        Window,
    };
    Array2<SimpleCell> data;

    struct RoomInfo
    {
        Vector2i position;
        Vector2i size;
        bool hallway;
    };

    std::vector<RoomInfo> rooms;
};

#endif//MAPGEN_H
