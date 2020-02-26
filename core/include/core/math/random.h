#pragma once

#include <random>
#include <type_traits>
#include "core/dg/math.h"


extern std::mt19937_64 randomGenerator;

void RandSeed(std::mt19937_64::result_type value);

template<typename T> typename std::enable_if<std::is_integral<T>::value, T>::type inline Rand() {
    return std::uniform_int_distribution<T>()(randomGenerator);
}

template<typename T> typename std::enable_if<std::is_floating_point<T>::value, T>::type inline Rand() {
    return std::uniform_real_distribution<T>()(randomGenerator);
}

template<typename T> typename std::enable_if<std::is_same<T, Diligent::float2>::value, T>::type inline Rand() {
    return T(Rand<float>(), Rand<float>());
}

template<typename T> typename std::enable_if<std::is_same<T, Diligent::float3>::value, T>::type inline Rand() {
    return T(Rand<float>(), Rand<float>(), Rand<float>());
}

template<typename T> typename std::enable_if<std::is_same<T, Diligent::float4>::value, T>::type inline Rand() {
    return T(Rand<float>(), Rand<float>(), Rand<float>(), Rand<float>());
}

template<typename T> typename std::enable_if<std::is_integral<T>::value, T>::type inline LinearRand(const T minValue, const T maxValue) {
    return std::uniform_int_distribution<T>(minValue, maxValue)(randomGenerator);
}

template<typename T> typename std::enable_if<std::is_floating_point<T>::value, T>::type inline LinearRand(const T minValue, const T maxValue) {
    return std::uniform_real_distribution<T>(minValue, maxValue)(randomGenerator);
}

template<typename T> typename std::enable_if<std::is_same<T, Diligent::float2>::value, T>::type inline LinearRand(const T& minValue, const T& maxValue) {
    return T(LinearRand<float>(minValue.x, maxValue.x), LinearRand<float>(minValue.y, maxValue.y));
}

template<typename T> typename std::enable_if<std::is_same<T, Diligent::float3>::value, T>::type inline LinearRand(const T& minValue, const T& maxValue) {
    return T(LinearRand<float>(minValue.x, maxValue.x), LinearRand<float>(minValue.y, maxValue.y), LinearRand<float>(minValue.z, maxValue.z));
}

template<typename T> typename std::enable_if<std::is_same<T, Diligent::float4>::value, T>::type inline LinearRand(const T& minValue, const T& maxValue) {
    return T(LinearRand<float>(minValue.x, maxValue.x), LinearRand<float>(minValue.y, maxValue.y), LinearRand<float>(minValue.z, maxValue.z), LinearRand<float>(minValue.w, maxValue.w));
}
