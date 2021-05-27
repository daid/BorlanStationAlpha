#include <stdio.h>
#include "frontend/frontend.h"
#include "cell.h"
#include "map.h"
#include "ecs.h"


EntityComponentSystem<int, float> ecs;

int main(int argc, char** argv)
{
    ecs.add(ecs.create(), 123);

    map.resize({20, 20});
    map(3, 3).type = Cell::WindowH;

    Frontend frontend;

    while(1)
    {
        auto size = frontend.beginDrawing();
        for(int y=0; y<std::min(size.y, map.size().y); y++)
        {
            for(int x=0; x<std::min(size.x, map.size().x); x++)
            {
                Cell& cell = map(x, y);
                cell.draw(frontend, x, y);
            }
        }
        frontend.present();

        auto input = frontend.getInput();
        if (input == INPUT_QUIT)
            break;
    }
    
    return 0;
}
