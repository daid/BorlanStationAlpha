#pragma once

#include "frontend/frontend.h"


class MapView
{
public:
    void draw(Frontend& frontend);

private:
    Vector2i camera_position;
};
