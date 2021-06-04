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
    msg_line_count = log_line_count;

    for(int x=0; x<size.x - 15; x++)
        frontend.draw(x, size.y - log_line_count - 1, '-', fg, bg);
    for(int y=0; y<size.y; y++)
        frontend.draw(size.x - 15, y, '|', fg, bg);

    frontend.fill({size.x-14, 0}, {14, size.y}, ' ', fg, bg);
    draw_stats(frontend, {size.x-14, 0}, {14, size.y});
    draw_log(frontend, {0, size.y - log_line_count}, {size.x - 15, log_line_count});
}

void HudView::draw_stats(Frontend& frontend, Vector2i p, Vector2i size)
{
    for(auto&& [e, position, health, inventory] : engine.query<Player, Position, Health, Inventory>()) {
        frontend.draw(p.x, p.y++, format("HP: @/@", health.current, health.max), fg);
        frontend.draw(p.x, p.y++, format("O2: @", int(map(position).oxygen * 100)), fg);

        p.y++;
        frontend.draw(p.x, p.y++, "Inventory:", fg);
        for(auto item : engine.upgrade(inventory.contents)) {
            frontend.draw(p.x+1, p.y++, item.get<Item>().name, fg);
        }

        p.y++;
        bool first = true;
        for(auto ce : map(position).entities) {
            if (ce.has<Item>()) {
                if (first) {
                    frontend.draw(p.x, p.y++, "On floor:", fg);
                    first = false;
                }
                frontend.draw(p.x+1, p.y++, ce.get<Item>().name, fg);
            }
        }
    }
}

void HudView::draw_log(Frontend& frontend, Vector2i position, Vector2i size)
{
    frontend.fill(position, size, ' ', fg, bg);

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
            if (y < 0)
                break;
        }
        if (y < 0)
            break;

        frontend.draw(position.x, position.y + y, msg, fg);
    }
    if (mlog.has_new_messages())
        frontend.draw(position.x + size.x - 6, position.y + size.y - 1, " MORE ", bg, fg);
}
