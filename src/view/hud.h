#pragma once

#include "view.h"


class HudView : public View
{
public:
    void draw(Frontend& frontend) override;

    int msg_line_count{0};
private:
    void draw_stats(Frontend& frontend, Vector2i position, Vector2i size);
    void draw_log(Frontend& frontend, Vector2i position, Vector2i size);
};
