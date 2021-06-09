#ifndef MAP_H
#define MAP_H

#include "cell.h"
#include "array2.h"
#include <functional>


class Map : public Array2<Cell>
{
public:
    template<typename T>
    bool has_entity_with(Vector2i position) {
        if (position.x < 0 || position.y < 0 || position.x >= size().x || position.y >= size().y)
            return false;
        for(auto e : (*this)(position).entities)
            if (e.has<T>())
                return true;
        return false;
    }

    using FovCallbackT = std::function<void(Vector2i)>;
    void visit_field_of_view(Vector2i center, int radius, const FovCallbackT& callback);

    //Get the amount of sight we have from a tile to a tile, in the range 0.0-1.0
    float line_of_sight(Vector2i start, Vector2i end);
private:
    void fov_step(Vector2i center, Vector2i dir, int radius, int row, float fmin, float fmax, const FovCallbackT& callback);
};

extern Map map;

#endif//MAP_H
