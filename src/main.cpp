#include <cstdio>
#include <cassert>
#include "frontend/frontend.h"
#include "cell.h"
#include "map.h"
#include "ecs.h"
#include "mapgen.h"

using ECS = EntityComponentSystem<int, float>;
ECS ecs;

int main(int argc, char** argv)
{
    auto e = ecs.create().set(123).set(1.1f);
    if (e)
        e.get<int>() = 5;
    assert(ECS::Entity() != e);

    for(auto&& [entity, value] : ecs.query<int>())
    {
        printf("%f %d\n", entity.get<float>(), value);
        value = 10;
    }
    if (e.has<float>())
        e.remove<int>();
    printf("Bla\n");

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
