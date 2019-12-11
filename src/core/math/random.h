#pragma once

#include <limits>
#include <cstdint>
#include <cstdlib>
#include <type_traits>

template <typename T> T Rand();

template <> uint8_t inline Rand<uint8_t>() {
    return (static_cast<uint8_t>(std::rand()) % std::numeric_limits<uint8_t>::max());
}

template <> uint16_t inline Rand<uint16_t>() {
    return
        (static_cast<uint16_t>(Rand<uint8_t>()) << static_cast<uint16_t>(8)) |
        (static_cast<uint16_t>(Rand<uint8_t>()) << static_cast<uint16_t>(0));
}

template <> uint32_t inline Rand<uint32_t>() {
    return
        (static_cast<uint32_t>(Rand<uint16_t>()) << static_cast<uint32_t>(16)) |
        (static_cast<uint32_t>(Rand<uint16_t>()) << static_cast<uint32_t>(0));
}

template <> uint64_t inline Rand<uint64_t>() {
    return
        (static_cast<uint64_t>(Rand<uint32_t>()) << static_cast<uint64_t>(32)) |
        (static_cast<uint64_t>(Rand<uint32_t>()) << static_cast<uint64_t>(0));
}

template <> float inline Rand<float>() {
    return (static_cast<float>(Rand<uint32_t>()) / static_cast<float>(std::numeric_limits<uint32_t>::max()));
}

template <> double inline Rand<double>() {
    return (static_cast<double>(Rand<uint64_t>()) / static_cast<double>(std::numeric_limits<uint64_t>::max()));
}

template <> inline Diligent::float2 Rand<Diligent::float2>() {
    return Diligent::float2(Rand<float>(), Rand<float>());
}

template <> inline Diligent::float3 Rand<Diligent::float3>() {
    return Diligent::float3(Rand<float>(), Rand<float>(), Rand<float>());
}

template <> inline Diligent::float4 Rand<Diligent::float4>() {
    return Diligent::float4(Rand<float>(), Rand<float>(), Rand<float>(), Rand<float>());
}

template<typename T> inline T LinearRand(T Min, T Max) {
    return Rand<T>() * (Max - Min) + Min;
}

template<> inline uint8_t LinearRand(uint8_t Min, uint8_t Max) {
    return Rand<uint8_t>() * (Max - Min + uint8_t(1)) + Min;
}

template<> inline uint16_t LinearRand(uint16_t Min, uint16_t Max) {
    return Rand<uint16_t>() * (Max - Min + uint16_t(1)) + Min;
}

template<> inline uint32_t LinearRand(uint32_t Min, uint32_t Max) {
    return Rand<uint32_t>() * (Max - Min + uint32_t(1)) + Min;
}

template<> inline uint64_t LinearRand(uint64_t Min, uint64_t Max) {
    return Rand<uint64_t>() * (Max - Min + uint64_t(1)) + Min;
}
