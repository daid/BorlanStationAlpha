#include "random.h"
#include <random>

static std::mt19937_64 gen;

int irandom(int min, int max)
{
    return std::uniform_int_distribution<>{min, max}(gen);
}
