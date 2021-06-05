#include "ai.h"
#include "components.h"
#include "random.h"


bool action_move(ECS::Entity e, Vector2i offset);

void run_ai_system()
{
    for(auto&& [e, rw, position] : engine.query<RandomWalk, Position>()) {
        if (rw.countdown) {
            rw.countdown--;
        } else {
            rw.countdown = rw.delay;

            action_move(e, Vector2i{irandom(-1, 1), irandom(-1, 1)});
        }
    }
}
