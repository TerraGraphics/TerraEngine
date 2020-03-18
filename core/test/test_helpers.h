#pragma once

#include <random>
#include <chrono>

#include "core/math/types.h"


inline std::mt19937_64 GetRandomGenerator() {
    auto seed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    return std::mt19937_64(static_cast<std::mt19937_64::result_type>(seed));
}

template<typename T> inline math::PlaneT<T> GetRandomPlane(std::mt19937_64 randomGenerator) {
    auto gen = std::uniform_real_distribution<T>(0.01f, 1.f);
    return math::PlaneT<T>(1.f, gen(randomGenerator), gen(randomGenerator), gen(randomGenerator));
}
