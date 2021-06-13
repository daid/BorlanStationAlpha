#include "selectposition.h"
#include "map.h"
#include "../map.h"
#include "traceline.h"


SelectPositionView::SelectPositionView(Vector2i _start, Vector2i _target)
: start(_start), target(_target)
{
}

void SelectPositionView::draw(Frontend& frontend)
{
    for(auto p : TraceLine(start, target)) {
        frontend.draw(p.x - MapView::camera_position.x, p.y - MapView::camera_position.y, '*', Color(1, 1, 1));
        if (p != start && map.has_entity_with<Solid>(p))
            break;
    }
    frontend.draw(target.x - MapView::camera_position.x, target.y - MapView::camera_position.y, 'X', Color(1, 1, 1));
}

std::optional<Vector2i> SelectPositionView::get_result(Frontend& frontend)
{
    while(true) {
        View::draw_views(frontend);

        auto input = frontend.get_input();
        if (input == INPUT_QUIT) return {};
        if (input == INPUT_CANCEL) return {};
        if (input == INPUT_LEFT) target.x--;
        if (input == INPUT_RIGHT) target.x++;
        if (input == INPUT_UP) target.y--;
        if (input == INPUT_DOWN) target.y++;
        if (input == INPUT_UP_LEFT) { target.y--; target.x--; }
        if (input == INPUT_DOWN_LEFT) { target.y++; target.x--; }
        if (input == INPUT_UP_RIGHT) { target.y--; target.x++; }
        if (input == INPUT_DOWN_RIGHT) { target.y++; target.x++; }
        if (input == '\r') return target;
        if (input == 'f') return target;
    }
}
