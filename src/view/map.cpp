#include "map.h"
#include "../map.h"
#include "components.h"


void MapView::draw(Frontend& frontend)
{
    auto size = frontend.get_draw_size();

    auto offset = Vector2i{(size.x - 15) / 2, (size.y - size.y / 8) / 2};
    for(auto&& [e, position] : engine.query<Player, Position>()) {
        camera_position = position - offset;
    }

    for(int y=std::max(0, -camera_position.y); y<std::min(size.y, map.size().y - camera_position.y); y++) {
        for(int x=std::max(0, -camera_position.x); x<std::min(size.x, map.size().x - camera_position.x); x++) {

            Cell& cell = map(camera_position + Vector2i{x, y});
            if (cell.visible && (cell.light_level.r > 0.1 || cell.light_level.g > 0.1 || cell.light_level.b > 0.1)) {
                frontend.setbg(x, y, cell.light_level * 0.1f);
                cell.last_seen_as = ' ';
                if (cell.floor) {
                    frontend.setbg(x, y, cell.light_level * 0.3f);
                    frontend.draw(x, y, '.', cell.light_level);
                    cell.last_seen_as = '.';
                }
                
                int shown_priority = std::numeric_limits<int>::min();
                for(auto e : cell.entities) {
                    if (e.has<Visual>()) {
                        auto visual = e.get<Visual>();
                        if (visual.priority > shown_priority) {
                            frontend.draw(x, y, visual.c, Color(visual.color) * cell.light_level);
                            cell.last_seen_as = visual.c;
                            shown_priority = visual.priority;
                        }
                    }
                }
            } else {
                frontend.draw(x, y, cell.last_seen_as, Color(0.2, 0.2, 0.2));
            }

            //frontend.setbg(x, y, Color(1, 1, 1) * cell.oxygen);
        }
    }
}
