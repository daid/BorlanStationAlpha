#include "hud.h"
#include "messagelog.h"
#include <array>


void HudView::draw(Frontend& frontend)
{
    draw_log(frontend);
}

void HudView::draw_log(Frontend& frontend)
{
    auto size = frontend.get_draw_size();
    Color fg(1, 1, 1);
    Color bg(0, 0, 0);

    int line_count = std::max(4, size.y / 8);
    for(int x=0; x<size.x; x++) {
        frontend.draw(x, size.y - line_count - 1, '-', fg, bg);
    }
    for(int n=0; n<line_count; n++) {
        for(int x=0; x<size.x; x++) {
            frontend.draw(x, size.y - line_count + n, ' ', fg, bg);
        }
    }

    for(int y=size.y-1, idx=0; y>size.y-line_count-1; y--) {
        auto msg = mlog.back(idx++);
        while(int(msg.size()) > size.x) {
            size_t split_idx = ((msg.size() - 1) / size.x) * size.x;
            while(split_idx > 0 && msg[split_idx] != ' ')
                split_idx -= 1;
            for(size_t x=0; split_idx+1+x < msg.size(); x++) {
                frontend.draw(x, y, msg[split_idx+1+x], fg, bg);
            }
            y--;
            if (y < size.y-line_count)
                return;
            msg = msg.substr(0, split_idx);
        }
        for(size_t x=0; x < msg.size(); x++) {
            frontend.draw(x, y, msg[x], fg, bg);
        }
    }
}
