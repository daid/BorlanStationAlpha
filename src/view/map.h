#pragma once

#include "view.h"


class MapView : public View
{
public:
    void draw(Frontend& frontend) override;

    static Vector2i camera_position;
};
