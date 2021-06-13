#pragma once

#include "vector2.h"


class TraceLine {
public:
    TraceLine(Vector2i _start, Vector2i _target)
    : start(_start), target(_target) {}

    class Iterator {
    public:
        Iterator(Vector2i _p, Vector2i target)
        : p(_p) {
            Vector2i delta = target - _p;
            fast = {delta.x > 0 ? 1 : -1, 0};
            slow = {0, delta.y > 0 ? 1 : -1};
            delta = {std::abs(delta.x), std::abs(delta.y)};
            if (delta.x < delta.y) {
                fast.y = slow.y;
                slow.x = fast.x;
                fast.x = slow.y = 0;
                err = 2*delta.x-delta.y;
                fast_err = 2*delta.y;
                slow_err = 2*delta.x;
            } else {
                err = 2*delta.y-delta.x;
                fast_err = 2*delta.x;
                slow_err = 2*delta.y;
            }
        }
        Iterator(Vector2i _p) : p(_p) {}
        Vector2i operator*() const { return p; }

        void operator++() {
            p += fast;
            if (err > 0) {
                p += slow;
                err -= fast_err;
            }
            err += slow_err;
        }
        bool operator!=(const Iterator& other) const {
            return p != other.p;
        }
    private:
        Vector2i p, fast, slow;
        int err;
        int fast_err, slow_err;
    };

    Iterator begin() {
        return {start, target};
    }
    Iterator end() {
        return {target};
    }

private:
    Vector2i start, target;
};
