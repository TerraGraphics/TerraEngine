#pragma once

#include <type_traits>

#include "core/math/generator_type.h"


namespace std {

// Min

template<typename T>
math::Generator2<T> min(const math::Generator2<T>& a, const math::Generator2<T>& b) {
    return math::Generator2<T>([a, b](T x, T y) -> T {
        return std::min(a(x, y), b(x, y));
    });
}

template<typename T, typename U, std::enable_if_t<math::GeneratorCompatibleType<U>, int> = 0>
math::Generator2<T> min(const math::Generator2<T>& a, U b) {
    return math::Generator2<T>([a, b = static_cast<T>(b)](T x, T y) -> T {
        return std::min(a(x, y), b);
    });
}

template<typename T>
math::Generator3<T> min(const math::Generator3<T>& a, const math::Generator3<T>& b) {
    return math::Generator3<T>([a, b](T x, T y, T z) -> T {
        return std::min(a(x, y, z), b(x, y, z));
    });
}

template<typename T, typename U, std::enable_if_t<math::GeneratorCompatibleType<U>, int> = 0>
math::Generator3<T> min(const math::Generator3<T>& a, U b) {
    return math::Generator3<T>([a, b = static_cast<T>(b)](T x, T y, T z) -> T {
        return std::min(a(x, y, z), b);
    });
}

// Max

template<typename T>
math::Generator2<T> max(const math::Generator2<T>& a, const math::Generator2<T>& b) {
    return math::Generator2<T>([a, b](T x, T y) -> T {
        return std::max(a(x, y), b(x, y));
    });
}

template<typename T, typename U, std::enable_if_t<math::GeneratorCompatibleType<U>, int> = 0>
math::Generator2<T> max(const math::Generator2<T>& a, U b) {
    return math::Generator2<T>([a, b = static_cast<T>(b)](T x, T y) -> T {
        return std::max(a(x, y), b);
    });
}

template<typename T>
math::Generator3<T> max(const math::Generator3<T>& a, const math::Generator3<T>& b) {
    return math::Generator3<T>([a, b](T x, T y, T z) -> T {
        return std::max(a(x, y, z), b(x, y, z));
    });
}

template<typename T, typename U, std::enable_if_t<math::GeneratorCompatibleType<U>, int> = 0>
math::Generator3<T> max(const math::Generator3<T>& a, U b) {
    return math::Generator3<T>([a, b = static_cast<T>(b)](T x, T y, T z) -> T {
        return std::max(a(x, y, z), b);
    });
}

}

namespace math {

// Operator +

template<typename T>
Generator2<T> operator+(const Generator2<T>& a, const Generator2<T>& b) {
    return Generator2<T>([a, b](T x, T y) -> T {
        return (a(x, y) + b(x, y));
    });
}

template<typename T, typename U, std::enable_if_t<GeneratorCompatibleType<U>, int> = 0>
Generator2<T> operator+(const Generator2<T>& a, U b) {
    return Generator2<T>([a, b = static_cast<T>(b)](T x, T y) -> T {
        return (a(x, y) + b);
    });
}

template<typename T>
Generator3<T> operator+(const Generator3<T>& a, const Generator3<T>& b) {
    return Generator3<T>([a, b](T x, T y, T z) -> T {
        return (a(x, y, z) + b(x, y, z));
    });
}

template<typename T, typename U, std::enable_if_t<GeneratorCompatibleType<U>, int> = 0>
Generator3<T> operator+(const Generator3<T>& a, U b) {
    return Generator3<T>([a, b = static_cast<T>(b)](T x, T y, T z) -> T {
        return (a(x, y, z) + b);
    });
}

}
