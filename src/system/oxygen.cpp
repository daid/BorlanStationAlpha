#include "oxygen.h"
#include "components.h"
#include "map.h"

static std::array<Vector2i, 8> DIRECTIONS = {{{-1,-1},{-1,0},{-1,1}, {0,-1},{0,1}, {1,-1},{1,0},{1,1}}};
//static std::array<Vector2i, 4> DIRECTIONS = {{{-1,0}, {1,0},{0,-1}, {0,1}}};

static void oxygenFlow(float factor)
{
    Array2<float> delta;
    delta.resize(map.size(), 0);

    for(int y=1; y<map.size().y-1; y++) {
        for(int x=1; x<map.size().x-1; x++) {
            Vector2i p{x, y};
            float diff_total{0};
            int flags{0};
            for(size_t n=0; n<DIRECTIONS.size(); n++) {
                if (map(p).oxygen > map(p + DIRECTIONS[n]).oxygen) {
                    if (!map.hasSolidEntity(p + DIRECTIONS[n])) {
                        flags |= (1 << n);
                        diff_total += map(p).oxygen - map(p + DIRECTIONS[n]).oxygen;
                    }
                }
            }

            if (diff_total > 0.0f) {
                float diff_transfer = std::min(map(p).oxygen, diff_total) * factor;
                float f = diff_transfer / diff_total;
                delta(p) -= diff_transfer;
                for(size_t n=0; n<DIRECTIONS.size(); n++) {
                    if (flags & (1 << n)) {
                        delta(p + DIRECTIONS[n]) += (map(p).oxygen - map(p + DIRECTIONS[n]).oxygen) * f;
                    }
                }
            }
        }
    }

    for(int y=1; y<map.size().y-1; y++) {
        for(int x=1; x<map.size().x-1; x++) {
            auto& tile = map(x, y);
            tile.oxygen += delta(x, y);
            if (!tile.floor)
                tile.oxygen *= 0.1f;
        }
    }
}

void runOxygenSystem()
{
    //First, agressively transfer oxygen between cells, but this leaves it in a very wave form pattern
    oxygenFlow(0.9);
    //So smooth out by transfering less in the next iterations.
    oxygenFlow(0.25);
    oxygenFlow(0.25);
    oxygenFlow(0.05);
    oxygenFlow(0.05);
}
