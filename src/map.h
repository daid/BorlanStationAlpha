#ifndef MAP_H
#define MAP_H

#include "cell.h"
#include "array2.h"
#include <functional>


class Map : public Array2<Cell>
{
public:
    bool hasSolidEntity(Vector2i position);
    bool visionBlocked(Vector2i position);

    using FovCallbackT = std::function<void(Vector2i)>;
    void visitFieldOfView(Vector2i center, int radius, const FovCallbackT& callback);

private:
    void fovStep(Vector2i center, Vector2i dir, int radius, int row, float fmin, float fmax, const FovCallbackT& callback);
};

extern Map map;

#endif//MAP_H
