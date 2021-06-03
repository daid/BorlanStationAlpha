#include "hud.h"
#include "../map.h"
#include "messagelog.h"
#include "components.h"
#include "format.h"
#include <array>
#include <iostream>


static const Color fg{1, 1, 1};
static const Color bg{0, 0, 0};


void HudView::draw(Frontend& frontend)
{
    auto size = frontend.get_draw_size();
    int log_line_count = std::max(4, size.y / 6);

    for(int x=0; x<size.x - 15; x++)
        frontend.draw(x, size.y - log_line_count - 1, '-', fg, bg);
    for(int y=0; y<size.y; y++)
        frontend.draw(size.x - 15, y, '|', fg, bg);

    clear(frontend, {size.x-14, 0}, {14, size.y});
    draw_stats(frontend, {size.x-14, 0}, {14, size.y});
    draw_log(frontend, {0, size.y - log_line_count}, {size.x - 15, log_line_count});
}

void HudView::clear(Frontend& frontend, Vector2i position, Vector2i size)
{
    for(int y=0; y<size.y; y++) {
        for(int x=0; x<size.x; x++) {
            frontend.draw(position.x + x, position.y + y, ' ', fg, bg);
        }
    }
}

void HudView::draw_stats(Frontend& frontend, Vector2i p, Vector2i size)
{
    for(auto&& [e, position, health] : engine.query<Player, Position, Health>()) {
        frontend.draw(p.x, p.y++, format("HP: @/@", health.current, health.max), fg);
        frontend.draw(p.x, p.y++, format("O2: @", map(position).oxygen), fg);
    }
}

void HudView::draw_log(Frontend& frontend, Vector2i position, Vector2i size)
{
    clear(frontend, position, size);

    for(int y=size.y-1, idx=0; y>=0; y--) {
        auto msg = mlog.back(idx++);
        std::vector<std::string_view> lines;
        while(int(msg.size()) > size.x) {
            size_t split = size.x;
            while(msg[split] != ' ' && split > 1)
                split--;
            lines.push_back(msg.substr(0, split));
            msg = msg.substr(split + 1);
        }
        for(auto line : lines) {
            frontend.draw(position.x, position.y + y, line, fg);
            y--;
            if (y < 0) return;
        }

        frontend.draw(position.x, position.y + y, msg, fg);
    }
}
