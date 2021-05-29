#include <cstdio>
#include <cassert>
#include "frontend/frontend.h"
#include "cell.h"
#include "map.h"
#include "ecs.h"
#include "mapgen.h"
#include "components.h"

ECS ecs;

int main(int argc, char** argv)
{
    Mapgen();

    auto e = ecs.create();
    e.set(Position{{12, 4}}).set(Visual{'@', HsvColor(0, 0, 100)}).set(Player{});

    Frontend frontend;

    while(1)
    {
        for(Cell& cell : map)
            cell.visible = false;
        map.visitFieldOfView({12, 4}, 20, [](Vector2i position)
        {
            map(position).visible = true;
        });

        auto size = frontend.beginDrawing();
        for(int y=0; y<std::min(size.y, map.size().y); y++)
        {
            for(int x=0; x<std::min(size.x, map.size().x); x++)
            {
                Cell& cell = map(x, y);
                if (cell.visible)
                {
                    frontend.setbg(x, y, Color(20, 20, 20));
                    if (cell.floor)
                        frontend.draw(x, y, '.', Color(255, 255, 255));
                }
            }
        }

        for(auto&& [entity, visual, position] : ecs.query<Visual, Position>())
        {
            Cell& cell = map(position);
            if (cell.visible)
                frontend.draw(position.x, position.y, visual.c, visual.color);
        }
        
        frontend.present();

        auto input = frontend.getInput();
        if (input == INPUT_QUIT)
            break;
    }
    
    return 0;
}
