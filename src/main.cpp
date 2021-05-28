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
    auto e = ecs.create().set(Position{{0, 0}}).set(Visual{'@', HsvColor(0, 100, 100)});
    e.destroy();

    Mapgen();

    Frontend frontend;

    while(1)
    {
        auto size = frontend.beginDrawing();
        for(int y=0; y<std::min(size.y, map.size().y); y++)
        {
            for(int x=0; x<std::min(size.x, map.size().x); x++)
            {
                Cell& cell = map(x, y);
                if (cell.floor)
                    frontend.draw(x, y, '.', Color(255, 255, 255));
            }
        }

        for(auto&& [entity, visual, position] : ecs.query<Visual, Position>())
        {
            frontend.draw(position.x, position.y, visual.c, visual.color);
        }
        
        frontend.present();

        auto input = frontend.getInput();
        if (input == INPUT_QUIT)
            break;
    }
    
    return 0;
}
