#pragma once

#include "frontend/frontend.h"


class View
{
public:
    View();
    virtual ~View();

    virtual void draw(Frontend& frontend) = 0;

    static void draw_views(Frontend& frontend);
};
