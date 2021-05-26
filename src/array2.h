#ifndef ARRAY2_H
#define ARRAY2_H

#include <vector>
#include "vector2.h"

template<typename T> class Array2
{
public:
    Vector2i size() const
    {
        return _size;
    }

    void resize(Vector2i new_size)
    {
        _size = new_size;
        data.clear();
        data.resize(_size.x * _size.y);
    }

    T& operator()(int x, int y)
    {
        return data[x + y * _size.x];
    }
private:
    Vector2i _size;
    std::vector<T> data;
};

#endif//ARRAY2_H
