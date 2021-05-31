#pragma once

#include <cstdlib>
#include <limits>

namespace ecs::detail {
    using IdType = std::size_t;
    static constexpr IdType no_entity = std::numeric_limits<IdType>::max();
}
