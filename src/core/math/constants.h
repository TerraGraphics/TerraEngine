#pragma once

#include <DiligentCore/Common/interface/BasicMath.h>


namespace Diligent {

inline const auto One4x4 = float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

inline const auto One3x3 = float3x3(
        1, 0, 0,
        0, 1, 0,
        0, 0, 1);

}

// pi * 2
template<typename T> inline constexpr T TwoPI() {
    return T(6.28318530717958647692528676655900576);
}

// pi
template<typename T> inline constexpr T PI() {
    return T(3.14159265358979323846264338327950288);
}

// pi / 2
template<typename T> inline constexpr T HalfPI() {
    return T(1.57079632679489661923132169163975144);
}

// pi / 4
template<typename T> inline constexpr T QuarterPI() {
    return T(0.785398163397448309615660845819875721);
}

// 1 / pi
template<typename T> inline constexpr T OneOverPI() {
    return T(0.318309886183790671537767526745028724);
}