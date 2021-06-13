#pragma once

#include "view.h"
#include <optional>


class SelectPositionView : public View
{
public:
    SelectPositionView(Vector2i start, Vector2i target);

    void draw(Frontend& frontend) override;
    std::optional<Vector2i> get_result(Frontend& frontend);
private:
    Vector2i start;
    Vector2i target;
};
