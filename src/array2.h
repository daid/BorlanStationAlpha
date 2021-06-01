#ifndef ARRAY2_H
#define ARRAY2_H

#include <vector>
#include "vector2.h"

template<typename T> class Array2
{
public:
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

    Vector2i size() const
    {
        return _size;
    }

    void resize(Vector2i new_size)
    {
        _size = new_size;
        data.clear();
        data.resize(_size.x * _size.y, {});
    }

    void resize(Vector2i new_size, const T& fill)
    {
        _size = new_size;
        data.clear();
        data.resize(_size.x * _size.y, fill);
    }

    T& operator()(Vector2i position)
    {
        return data[position.x + position.y * _size.x];
    }

    T& operator()(int x, int y)
    {
        return data[x + y * _size.x];
    }

    const T& operator()(Vector2i position) const
    {
        return data[position.x + position.y * _size.x];
    }

    const T& operator()(int x, int y) const
    {
        return data[x + y * _size.x];
    }

    iterator begin() { return data.begin(); }
    iterator end() { return data.end(); }
    const_iterator begin() const { return data.begin(); }
    const_iterator end() const { return data.end(); }

private:
    Vector2i _size;
    std::vector<T> data;
};

#endif//ARRAY2_H
