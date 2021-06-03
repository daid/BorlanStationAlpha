#pragma once

#include "frontend/frontend.h"


class HudView
{
public:
    void draw(Frontend& frontend);

private:
    void draw_log(Frontend& frontend);
};
