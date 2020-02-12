#include "core/math/random.h"

std::mt19937_64 randomGenerator = std::mt19937_64(0);

void RandSeed(std::mt19937_64::result_type value) {
    randomGenerator.seed(value);
}
