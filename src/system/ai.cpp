#include "ai.h"
#include "components.h"
#include "random.h"
#include "map.h"
#include "messagelog.h"
#include "action/move.h"


void AISystem::run()
{
    for(auto&& [player, player_position] : engine.query<Player, Position>()) {
        for(auto&& [e, enemy, position] : engine.query<Enemy, Position>()) {
            if (enemy.delay_turns > 0) enemy.delay_turns--;
            if (enemy.delay_turns != 0) continue;

            switch(enemy.state) {
            case Enemy::Wander:
                if (map.line_of_sight(position, player_position) > 0.1) {
                    enemy.state = Enemy::Attack;
                    enemy.delay_turns = 40;
                }
                break;
            case Enemy::Attack:
                if (e.has<MeleeAttack>()) {
                    if (map.line_of_sight(position, player_position) > 0.1) {
                        Vector2i offset{0, 0};
                        if (position.x < player_position.x) offset.x = 1;
                        if (position.x > player_position.x) offset.x =-1;
                        if (position.y < player_position.y) offset.y = 1;
                        if (position.y > player_position.y) offset.y =-1;

                        enemy.delay_turns = action_move(e, offset) * 1.2;
                    }
                }
                break;
            }
        }
    }
}
