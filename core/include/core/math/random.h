#pragma once

#include <random>
#include <type_traits>

#include "dg/math.h"


extern std::mt19937_64 randomGenerator;

void RandSeed(std::mt19937_64::result_type value);

template<typename T> typename std::enable_if_t<std::is_integral_v<T>, T> inline Rand() {
    return std::uniform_int_distribution<T>()(randomGenerator);
}

template<typename T> typename std::enable_if_t<std::is_floating_point_v<T>, T> inline Rand() {
    return std::uniform_real_distribution<T>()(randomGenerator);
}

template<typename T> typename std::enable_if_t<std::is_same_v<T, dg::float2>, T> inline Rand() {
    return T(Rand<float>(), Rand<float>());
}

template<typename T> typename std::enable_if_t<std::is_same_v<T, dg::float3>, T> inline Rand() {
    return T(Rand<float>(), Rand<float>(), Rand<float>());
}

template<typename T> typename std::enable_if_t<std::is_same_v<T, dg::float4>, T> inline Rand() {
    return T(Rand<float>(), Rand<float>(), Rand<float>(), Rand<float>());
}

template<typename T> typename std::enable_if_t<std::is_integral_v<T>, T> inline LinearRand(const T minValue, const T maxValue) {
    return std::uniform_int_distribution<T>(minValue, maxValue)(randomGenerator);
}

template<typename T> typename std::enable_if_t<std::is_floating_point_v<T>, T> inline LinearRand(const T minValue, const T maxValue) {
    return std::uniform_real_distribution<T>(minValue, maxValue)(randomGenerator);
}

template<typename T> typename std::enable_if_t<std::is_same_v<T, dg::float2>, T> inline LinearRand(const T& minValue, const T& maxValue) {
    return T(LinearRand<float>(minValue.x, maxValue.x), LinearRand<float>(minValue.y, maxValue.y));
}

template<typename T> typename std::enable_if_t<std::is_same_v<T, dg::float3>, T> inline LinearRand(const T& minValue, const T& maxValue) {
    return T(LinearRand<float>(minValue.x, maxValue.x), LinearRand<float>(minValue.y, maxValue.y), LinearRand<float>(minValue.z, maxValue.z));
}

template<typename T> typename std::enable_if_t<std::is_same_v<T, dg::float4>, T> inline LinearRand(const T& minValue, const T& maxValue) {
    return T(LinearRand<float>(minValue.x, maxValue.x), LinearRand<float>(minValue.y, maxValue.y), LinearRand<float>(minValue.z, maxValue.z), LinearRand<float>(minValue.w, maxValue.w));
}
