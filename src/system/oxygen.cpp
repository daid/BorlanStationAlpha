#include "oxygen.h"
#include "components.h"
#include "map.h"

static void oxygenFlow(float factor)
{
    Array2<float> delta;
    delta.resize(map.size(), 0);

    for(int y=1; y<map.size().y-1; y++) {
        for(int x=1; x<map.size().x-1; x++) {
            float diff_total{0};
            int flags{0};
            if (!map.hasSolidEntity({x + 1, y})) flags |= 1;
            if (!map.hasSolidEntity({x - 1, y})) flags |= 2;
            if (!map.hasSolidEntity({x, y + 1})) flags |= 4;
            if (!map.hasSolidEntity({x, y - 1})) flags |= 8;
            if (flags & 1) diff_total += std::max(0.0f, map(x, y).oxygen - map(x + 1, y).oxygen);
            if (flags & 2) diff_total += std::max(0.0f, map(x, y).oxygen - map(x - 1, y).oxygen);
            if (flags & 4) diff_total += std::max(0.0f, map(x, y).oxygen - map(x, y + 1).oxygen);
            if (flags & 8) diff_total += std::max(0.0f, map(x, y).oxygen - map(x, y - 1).oxygen);

            if (diff_total > 0.0f) {
                float diff_transfer = std::min(map(x, y).oxygen, diff_total) * factor;
                float f = diff_transfer / diff_total;

                delta(x, y) -= diff_transfer;
                if (flags & 1) delta(x + 1, y) += std::max(0.0f, map(x, y).oxygen - map(x + 1, y).oxygen) * f;
                if (flags & 2) delta(x - 1, y) += std::max(0.0f, map(x, y).oxygen - map(x - 1, y).oxygen) * f;
                if (flags & 4) delta(x, y + 1) += std::max(0.0f, map(x, y).oxygen - map(x, y + 1).oxygen) * f;
                if (flags & 8) delta(x, y - 1) += std::max(0.0f, map(x, y).oxygen - map(x, y - 1).oxygen) * f;
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
    oxygenFlow(0.9);
    oxygenFlow(0.9);
    oxygenFlow(0.25);
    oxygenFlow(0.25);
    oxygenFlow(0.25);
}
