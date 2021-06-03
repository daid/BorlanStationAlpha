#pragma once

#include "frontend/frontend.h"


class HudView
{
public:
    void draw(Frontend& frontend);

private:
    void clear(Frontend& frontend, Vector2i position, Vector2i size);
    void draw_stats(Frontend& frontend, Vector2i position, Vector2i size);
    void draw_log(Frontend& frontend, Vector2i position, Vector2i size);
};
