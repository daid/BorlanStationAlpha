#include "hud.h"
#include <array>


static const std::array<std::string, 2> log_messages{"123 ", "456"};


void HudView::draw(Frontend& frontend)
{
    auto size = frontend.getDrawSize();
    Color fg(1, 1, 1);
    Color bg(0.0, 0.0, 0.0);

    int line_count = size.y / 8;
    for(int n=0; n<line_count; n++) {
        for(int x=0; x<size.x; x++) {
            frontend.draw(x, size.y - line_count + n, ' ', fg, bg);
        }
    }
    for(int n=0; n<line_count; n++) {
        int y = size.y - 1 - n;
        if (int(log_messages.size()) - 1 - n < 0)
            break;
        const std::string& msg = log_messages[log_messages.size() - 1 - n];
        for(int x=0; msg[x]; x++) {
            frontend.draw(x, y, msg[x], fg, bg);
        }
    }
}
