#include "oxygen.h"
#include "components.h"
#include "map.h"
#include "messagelog.h"
#include "health.h"

static const std::array<Vector2i, 8> DIRECTIONS = {{{-1,-1},{-1,0},{-1,1}, {0,-1},{0,1}, {1,-1},{1,0},{1,1}}};


void OxygenSystem::run()
{
    for(auto&& [e, organic, position] : engine.query<Organic, Position>()) {
        organic.suffocate_count++;
        if (map(position).oxygen > organic.oxygen_usage * 5.0f) {
            map(position).oxygen -= organic.oxygen_usage;
            organic.suffocate_count = 0;
        } else if (e.has<Wearing>()) {
            auto suit = engine.upgrade(e.get<Wearing>());
            if (suit.has<OxygenStorage>() && suit.get<OxygenStorage>().current > 0.0) {
                auto& o2storage = suit.get<OxygenStorage>();
                o2storage.current = std::max(0.0f, o2storage.current - organic.oxygen_usage);
                organic.suffocate_count = 0;
            }
        }
        if (organic.suffocate_count > 0 && e.has<Player>())
            mlog.add("You cannot breathe!");
        if (organic.suffocate_count > 4)
            HealthSystem::takeDamage(e, DamageType::Suffocate, 5 * (organic.suffocate_count / 4));
    }

    airtight.resize(map.size(), 0);
    for(auto&& [e, position] : engine.query<Airtight, Position>()) {
        airtight(position) = 1;
    }

    //First, agressively transfer oxygen between cells, but this leaves it in a very wave form pattern
    oxygen_flow(0.9);
    oxygen_flow(0.9);
    //So smooth out by transfering less in the next iterations.
    oxygen_flow(0.25);
    oxygen_flow(0.25);
    oxygen_flow(0.25);
    oxygen_flow(0.25);
    oxygen_flow(0.05);
    oxygen_flow(0.05);
}

void OxygenSystem::oxygen_flow(float factor)
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
                    if (!airtight(p + DIRECTIONS[n])) {
                        flags |= (1 << n);
                        diff_total += map(p).oxygen - map(p + DIRECTIONS[n]).oxygen;
                    }
                }
            }

            if (diff_total > 0.01f) {
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
