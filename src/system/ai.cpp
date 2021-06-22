#include "ai.h"
#include "components.h"
#include "random.h"
#include "map.h"
#include "messagelog.h"
#include "action/move.h"
#include <queue>


class PathFind {
public:
    Vector2i search(Vector2i start, Vector2i target) {
        target_distance.resize(map.size(), std::numeric_limits<int>::max());
        todo.emplace(target, 0);
        while(!todo.empty() && target_distance(start) == std::numeric_limits<int>::max()) {
            fill(std::get<0>(todo.front()), std::get<1>(todo.front()));
            todo.pop();
        }

        if (target_distance(start) == std::numeric_limits<int>::max())
            return {0, 0};
        
        int best = std::numeric_limits<int>::max();
        Vector2i result{0, 0};
        if (target.x > 0 && target_distance(start - Vector2i{1, 0}) < best) { best = target_distance(start - Vector2i{1, 0}); result = {-1, 0}; }
        if (target.y > 0 && target_distance(start - Vector2i{0, 1}) < best) { best = target_distance(start - Vector2i{0, 1}); result = {0, -1}; }
        if (target.x < map.size().x - 1 && target_distance(start + Vector2i{1, 0}) < best) { best = target_distance(start + Vector2i{1, 0}); result = {1, 0}; }
        if (target.y < map.size().y - 1 && target_distance(start + Vector2i{0, 1}) < best) { best = target_distance(start + Vector2i{0, 1}); result = {0, 1}; }

        if (target.x > 0 && target.y > 0 && target_distance(start + Vector2i{-1, -1}) < best) { best = target_distance(start + Vector2i{-1, -1}); result = {-1, -1}; }
        if (target.x > 0 && target.y < map.size().y - 1 && target_distance(start + Vector2i{-1, 1}) < best) { best = target_distance(start + Vector2i{-1, 1}); result = {-1, 1}; }
        if (target.x < map.size().x - 1 && target.y > 0 && target_distance(start + Vector2i{1, -1}) < best) { best = target_distance(start + Vector2i{1, -1}); result = {1, -1}; }
        if (target.x < map.size().x - 1 && target.y < map.size().y - 1 && target_distance(start + Vector2i{1, 1}) < best) { best = target_distance(start + Vector2i{1, 1}); result = {1, 1}; }
        return result;
    }

    void fill(Vector2i position, int distance) {
        if (target_distance(position) <= distance) return;
        if (map.has_entity_with<Solid>(position)) return;
        target_distance(position) = distance;
        if (position.x > 0) todo.emplace(position - Vector2i{1, 0}, distance + 1);
        if (position.x < map.size().x - 1) todo.emplace(position + Vector2i{1, 0}, distance + 1);
        if (position.y > 0) todo.emplace(position - Vector2i{0, 1}, distance + 1);
        if (position.y < map.size().y - 1) todo.emplace(position + Vector2i{0, 1}, distance + 1);
    }
private:
    std::queue<std::tuple<Vector2i, int>> todo;
    Array2<int> target_distance;
};




void AISystem::run()
{
    for(auto&& [player, player_position] : engine.query<Player, Position>()) {
        for(auto&& [e, enemy, position] : engine.query<Enemy, Position>()) {
            if (enemy.delay_turns > 0) enemy.delay_turns--;
            if (enemy.delay_turns != 0) continue;

            bool can_see_player = map.line_of_sight(position, player_position) > 0.1;
            if (can_see_player)
                enemy.last_known_target_position = player_position;

            switch(enemy.state) {
            case Enemy::Wander:
                if (can_see_player) {
                    enemy.state = Enemy::Attack;
                    enemy.delay_turns = 40;
                }
                break;
            case Enemy::Attack:
                if (can_see_player) {
                    if (e.has<MeleeAttack>()) {
                        Vector2i offset = PathFind().search(position, player_position);

                        enemy.delay_turns = action_move(e, offset) * 1.2;
                    }
                } else {
                    enemy.state = Enemy::Seek;
                    enemy.delay_turns = 10;
                }
                break;
            case Enemy::Seek:
                if (can_see_player) {
                    enemy.state = Enemy::Attack;
                    enemy.delay_turns = 20;
                } else {
                    Vector2i offset = PathFind().search(position, enemy.last_known_target_position);
                    enemy.delay_turns = action_move(e, offset) * 1.2;
                }
                break;
            }
        }
    }
}
