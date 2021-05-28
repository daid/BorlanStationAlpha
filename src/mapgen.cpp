#include "mapgen.h"
#include "map.h"
#include <random>


Mapgen::Mapgen()
{
    std::mt19937_64 gen;

    map.resize({80, 80});

    enum Direction {
        Up, Down, Left, Right,
    };
    struct DoorInfo{
        Vector2i position;
        Direction direction;
    };
    std::vector<DoorInfo> door_options;

    Vector2i position{30, 30};
    Vector2i size{5, 5};
    addRoom(position, size);
    door_options.push_back({position + Vector2i(size.x / 2, 0), Up});
    door_options.push_back({position + Vector2i(size.x / 2, size.y - 1), Down});
    door_options.push_back({position + Vector2i(0, size.x / 2), Left});
    door_options.push_back({position + Vector2i(size.x - 1, size.y / 2), Right});

    while(!door_options.empty())
    {
        int idx = std::uniform_int_distribution<>{0, int(door_options.size() - 1)}(gen);
        auto door = door_options[idx];
        door_options[idx] = door_options[door_options.size() - 1];
        door_options.pop_back();

        size.x = std::uniform_int_distribution<>{3, 10}(gen);
        size.y = std::uniform_int_distribution<>{3, 10}(gen);
        position = door.position;
        switch(door.direction)
        {
        case Up: position.x -= size.x / 2; position.y -= size.y; break;
        case Down: position.x -= size.x / 2; position.y += 1; break;
        case Left: position.x -= size.x; position.y -= size.y / 2; break;
        case Right: position.y -= size.y / 2; position.x += 1; break;
        }
        if (addRoom(position, size))
        {
            door_options.push_back({position + Vector2i(size.x / 2, 0), Up});
            door_options.push_back({position + Vector2i(size.x / 2, size.y - 1), Down});
            door_options.push_back({position + Vector2i(0, size.x / 2), Left});
            door_options.push_back({position + Vector2i(size.x - 1, size.y / 2), Right});
        }
    }
}

bool Mapgen::addRoom(Vector2i position, Vector2i size)
{
    if (position.x < 0 || position.y < 0 || position.x + size.x >= map.size().x || position.y + size.y >= map.size().y)
        return false;
    for(int x=0; x<size.x; x++)
        for(int y=0; y<size.y; y++)
            if (map(position.x + x, position.y + y).floor)
                return false;
    for(int x=0; x<size.x; x++)
        for(int y=0; y<size.y; y++)
            map(position.x + x, position.y + y).floor = true;
    rooms.push_back({position, size});
    return true;
}
