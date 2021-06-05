#include "mapgen.h"
#include "map.h"
#include "components.h"
#include "random.h"


Mapgen::Mapgen()
{
    map.resize({80, 80});
    data.resize(map.size(), Unset);
    for(int x=0; x<data.size().x; x++) {
        data(x, 0) = Vacuum;
        data(x, data.size().y - 1) = Vacuum;
    }
    for(int y=0; y<data.size().y; y++) {
        data(0, y) = Vacuum;
        data(data.size().x - 1, y) = Vacuum;
    }

    enum Direction {
        Up, Down, Left, Right,
    };
    struct DoorInfo{
        Vector2i position;
        Direction direction;
        bool expect_hallway;
    };
    std::vector<DoorInfo> door_options;

    Vector2i position{irandom(10, map.size().x - 20), irandom(10, map.size().y - 20)};
    Vector2i size{irandom(5, 10), irandom(5, 10)};
    start = position + size / 2;
    add_room(position, size);
    door_options.push_back({position + Vector2i{size.x / 2, 0}, Up, true});
    door_options.push_back({position + Vector2i{size.x / 2, size.y - 1}, Down, true});
    door_options.push_back({position + Vector2i{0, size.x / 2}, Left, true});
    door_options.push_back({position + Vector2i{size.x - 1, size.y / 2}, Right, true});

    while(!door_options.empty()) {
        int idx = irandom(0, int(door_options.size() - 1));
        auto door = door_options[idx];
        door_options[idx] = door_options[door_options.size() - 1];
        door_options.pop_back();

        size.x = irandom(5, 12);
        size.y = irandom(5, 12);
        if (door.expect_hallway && irandom(0, 100) < 40) door.expect_hallway = false;
        if (door.expect_hallway) {
            size.x = size.y = std::max(size.x, size.y);
            if (irandom(0, 100) < 20) size *= 2;
            if (door.direction == Up || door.direction == Down) size.x = 4; else size.y = 4;
        }
        position = door.position;
        switch(door.direction) {
        case Up: position.x -= size.x / 2; position.y -= size.y; break;
        case Down: position.x -= size.x / 2; position.y += 1; break;
        case Left: position.x -= size.x; position.y -= size.y / 2; break;
        case Right: position.y -= size.y / 2; position.x += 1; break;
        }
        if (add_room(position, size)) {
            if (door.expect_hallway) {
                rooms.back().hallway = true;
                if (door.direction == Up || door.direction == Down) {
                    door_options.push_back({position + Vector2i{irandom(1, size.x - 2), 0}, Up, false});
                    door_options.push_back({position + Vector2i{irandom(1, size.x - 2), size.y - 1}, Down, false});
                } else {
                    door_options.push_back({position + Vector2i{0, irandom(1, size.y - 2)}, Left, false});
                    door_options.push_back({position + Vector2i{size.x - 1, irandom(1, size.y - 2)}, Right, false});
                }
            } else {
                door_options.push_back({position + Vector2i{irandom(1, size.x - 2), 0}, Up, true});
                door_options.push_back({position + Vector2i{irandom(1, size.x - 2), size.y - 1}, Down, true});
                door_options.push_back({position + Vector2i{0, irandom(1, size.y - 2)}, Left, true});
                door_options.push_back({position + Vector2i{size.x - 1, irandom(1, size.y - 2)}, Right, true});
            }
        }
    }

    for(auto& room : rooms) {
        if (room.hallway) {
            //Check if hallways have at least two connections.
            int flags = 0;
            for(int x=1; x<room.size.x-1; x++) {
                if (data(room.position.x+x, room.position.y-1) > Vacuum) flags |= 0x01;
                if (data(room.position.x+x, room.position.y+room.size.y) > Vacuum) flags |= 0x02;
            }
            for(int y=1; y<room.size.y-1; y++) {
                if (data(room.position.x-1, room.position.y+y) > Vacuum) flags |= 0x04;
                if (data(room.position.x+room.size.x, room.position.y+y) > Vacuum) flags |= 0x08;
            }
            if (flags == 0x01 || flags == 0x02 || flags == 0x04 || flags == 0x08) {
                //Single connected hallway, check if extending it would connect it.
                auto dir = Vector2i{1, 0};
                auto pos = room.position;
                if (flags == 0x01) { dir = Vector2i{ 0, 1}; pos += Vector2i{irandom(1, room.size.x - 2), room.size.y}; }
                if (flags == 0x02) { dir = Vector2i{ 0,-1}; pos += Vector2i{irandom(1, room.size.x - 2), -1}; }
                if (flags == 0x04) { dir = Vector2i{ 1, 0}; pos += Vector2i{room.size.x, irandom(1, room.size.y - 2)}; }
                if (flags == 0x08) { dir = Vector2i{-1, 0}; pos += Vector2i{-1, irandom(1, room.size.y - 2)}; }

                int extend = 0;
                for(int n=0; n<15; n++) {
                    auto p = pos + dir * (n + 1);
                    if (p.x < 0 || p.x >= data.size().x) break;
                    if (p.y < 0 || p.y >= data.size().y) break;
                    if (data(p) > Vacuum) { extend = n + 1; break; }
                }

                if (extend)
                {
                    if (flags == 0x01) { room.size.y += extend; }
                    if (flags == 0x02) { room.position.y -= extend; room.size.y += extend; }
                    if (flags == 0x04) { room.size.x += extend; }
                    if (flags == 0x08) { room.position.x -= extend; room.size.x += extend; }
                    for(int y=0; y<room.size.y; y++)
                        for(int x=0; x<room.size.x; x++)
                            data(room.position + Vector2i{x, y}) = Floor;
                } else {
                    for(int y=0; y<room.size.y; y++)
                        for(int x=0; x<room.size.x; x++)
                            data(room.position + Vector2i{x, y}) = Unset;
                    room.size = {0, 0};
                    continue;
                }
            }
        }
    }
    rooms.erase(std::remove_if(rooms.begin(), rooms.end(), [](const RoomInfo& r) {return r.size.x == 0;}), rooms.end());

    //Create walls for each room
    for(auto& room : rooms) {
        for(int x=room.position.x; x<room.position.x+room.size.x; x++) {
            set_wall_h({x, room.position.y});
            set_wall_h({x, room.position.y+room.size.y-1});
        }
        for(int y=room.position.y; y<room.position.y+room.size.y; y++)
        {
            set_wall_v({room.position.x, y});
            set_wall_v({room.position.x+room.size.x-1, y});
        }
    }

    // Find where to put doors
    for(auto& room : rooms) {
        std::vector<std::vector<Vector2i>> options;
        options.emplace_back();
        for(int x=room.position.x; x<room.position.x+room.size.x; x++) {
            if (is_wall_for_door({x, room.position.y}))
                options.back().push_back({x, room.position.y});
            else if (is_wall_for_door({x, room.position.y - 1}))
                options.back().push_back({x, room.position.y - 1});
            else if (!options.back().empty())
                options.emplace_back();
        }
        options.emplace_back();
        for(int y=room.position.y; y<room.position.y+room.size.y; y++) {
            if (is_wall_for_door({room.position.x, y}))
                options.back().push_back({room.position.x, y});
            else if (is_wall_for_door({room.position.x - 1, y}))
                options.back().push_back({room.position.x - 1, y});
            else if (!options.back().empty())
                options.emplace_back();
        }
        for(auto& o : options)
        {
            bool done = o.size() == 0;
            for(auto p : o)
                if (data(p) == Door)
                    done = true;
            if (!done) {
                data(o[irandom(0, o.size()-1)]) = Door;
            }
        }
    }

    for(auto& room : rooms) {
        if (!room.hallway) continue;

        std::vector<Vector2i> options;
        for(int x=room.position.x+1; x<room.position.x+room.size.x-1; x++) {
            if (is_wall_for_window({x, room.position.y}))
                options.push_back({x, room.position.y});
            if (is_wall_for_window({x, room.position.y+room.size.y-1}))
                options.push_back({x, room.position.y+room.size.y-1});
        }
        for(int y=room.position.y+1; y<room.position.y+room.size.y-1; y++) {
            if (is_wall_for_window({room.position.x, y}))
                options.push_back({room.position.x, y});
            if (is_wall_for_window({room.position.x+room.size.x-1, y}))
                options.push_back({room.position.x+room.size.x-1, y});
        }
        for(auto p : options)
            data(p) = ThinWall;
        for(size_t n=0; n<options.size() / 5; n++)
        {
            auto p = options[irandom(0, options.size()-1)];
            data(p) = Window;
            if (data(p + Vector2i{1, 0}) == ThinWall)
                data(p + Vector2i{1, 0}) = Window;
            if (data(p + Vector2i{0, 1}) == ThinWall)
                data(p + Vector2i{0, 1}) = Window;
        }
    }



    // Add contents in a room
    for(auto& room : rooms) {
        auto light_count = (room.size + Vector2i{7, 7}) / 8;
        Vector2i spacing{room.size.x / light_count.x, room.size.y / light_count.y};
        Vector2i offset = room.position + Vector2i{(room.size.x - ((light_count.x - 1) * spacing.x)) / 2, (room.size.y - ((light_count.y - 1) * spacing.y)) / 2};

        int light_off = irandom(0, light_count.x * light_count.y - 1);
        if (irandom(0, 100) > 25 * light_count.x * light_count.y) light_off = -1;
        for(int y=0; y<light_count.y; y++) {
            for(int x=0; x<light_count.x; x++) {
                if (x + y * light_count.x != light_off)
                    engine.create().set(Light{4.0f, HsvColor(irandom(0, 360), 0, 100)}).set(Position{offset + Vector2i{spacing.x * x, spacing.y * y}});
            }
        }

        engine.create().set(Item{"TestItem"}).set(Visual{'!', HsvColor(0, 100, 100), -1}).set(Position{room.position + room.size / 2});
        engine.create().set<Solid>().set(Visual{'S', HsvColor(240, 100, 100)}).set(Position{room.position + room.size / 2}).set(RandomWalk{0, 2});
    }


    for(int y=0; y<data.size().y; y++) {
        for(int x=0; x<data.size().x; x++) {
            map(x, y).floor = true;
            switch(data(x, y)) {
            case Unset: map(x, y).floor = false; break;
            case Vacuum: map(x, y).floor = false; break;
            case Floor: map(x, y).oxygen = 1.0; break;
            case Wall: engine.create().set<Solid>().set<BlockVision>().set(Position{{x, y}}).set(Visual{'#', {0, 0, 100}}); break;
            case ThinWall:
                if (data(x - 1, y) <= Vacuum || data(x + 1, y) <= Vacuum)
                    engine.create().set<Solid>().set<BlockVision>().set(Position{{x, y}}).set(Visual{'|', {0, 0, 100}});
                else
                    engine.create().set<Solid>().set<BlockVision>().set(Position{{x, y}}).set(Visual{'-', {0, 0, 100}});
                break;
            case Door: engine.create().set(Position{{x, y}}).set(Visual{'+', {30, 50, 100}}).set<Solid>(); break;
            case Window: engine.create().set<Solid>().set(Position{{x, y}}).set(Visual{'+', {180, 80, 100}}); break;
            }
        }
    }
}

bool Mapgen::add_room(Vector2i position, Vector2i size)
{
    if (position.x < 0 || position.y < 0 || position.x + size.x >= map.size().x || position.y + size.y >= map.size().y)
        return false;
    for(int x=0; x<size.x; x++)
        for(int y=0; y<size.y; y++)
            if (data(position.x + x, position.y + y) != Unset)
                return false;
    for(int x=0; x<size.x; x++)
        for(int y=0; y<size.y; y++)
            data(position.x + x, position.y + y) = Floor;
    rooms.push_back({position, size, false});
    return true;
}

bool Mapgen::is_wall_for_door(Vector2i position)
{
    if (data(position) == Door) return true;
    if (data(position) != Wall) return false;
    if (data(position + Vector2i{0, -1}) == Floor && data(position + Vector2i{0, 1}) == Floor) return true;
    if (data(position + Vector2i{-1, 0}) == Floor && data(position + Vector2i{1, 0}) == Floor) return true;
    return false;
}

bool Mapgen::is_wall_for_window(Vector2i position)
{
    if (data(position) != Wall) return false;
    if (data(position + Vector2i{0, -1}) == Floor && data(position + Vector2i{ 0, 1}) <= Vacuum) return true;
    if (data(position + Vector2i{0,  1}) == Floor && data(position + Vector2i{ 0,-1}) <= Vacuum) return true;
    if (data(position + Vector2i{-1, 0}) == Floor && data(position + Vector2i{ 1, 0}) <= Vacuum) return true;
    if (data(position + Vector2i{ 1, 0}) == Floor && data(position + Vector2i{-1, 0}) <= Vacuum) return true;
    return false;
}

void Mapgen::set_wall_h(Vector2i position)
{
    if (data(position+Vector2i{-1,-1}) == Wall && data(position+Vector2i{0,-1}) == Wall && data(position+Vector2i{1,-1}) == Wall) return;
    if (data(position+Vector2i{-1, 1}) == Wall && data(position+Vector2i{0, 1}) == Wall && data(position+Vector2i{1, 1}) == Wall) return;
    data(position) = Wall;
}

void Mapgen::set_wall_v(Vector2i position)
{
    if (data(position+Vector2i{-1, 1}) == Wall && data(position+Vector2i{-1, 0}) == Wall && data(position+Vector2i{-1,-1}) == Wall) return;
    if (data(position+Vector2i{ 1, 1}) == Wall && data(position+Vector2i{ 1, 0}) == Wall && data(position+Vector2i{ 1,-1}) == Wall) return;
    data(position) = Wall;
}
