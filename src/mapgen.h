#ifndef MAPGEN_H
#define MAPGEN_H

#include "map.h"

class Mapgen
{
public:
    Mapgen();

private:
    void set_wall_h(Vector2i position);
    void set_wall_v(Vector2i position);

    bool add_room(Vector2i position, Vector2i size);

    bool is_wall_for_door(Vector2i position);
    bool is_wall_for_window(Vector2i position);

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
