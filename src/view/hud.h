#pragma once

#include "frontend/frontend.h"


class HudView
{
public:
    void draw(Frontend& frontend);

    int msg_line_count{0};
private:
    void clear(Frontend& frontend, Vector2i position, Vector2i size);
    void draw_stats(Frontend& frontend, Vector2i position, Vector2i size);
    void draw_log(Frontend& frontend, Vector2i position, Vector2i size);
};
