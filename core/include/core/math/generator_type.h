#pragma once

#include <functional>
#include <type_traits>

#include "core/common/meta.h"
#include "core/math/generator_type_fwd.h"


namespace math {

template<typename T>
    constexpr bool GeneratorCompatibleType = std::is_floating_point_v<T>;

template <typename T, typename Enable = std::enable_if_t<GeneratorCompatibleType<T>>>
    class Generator2 {
public:
    using Type = T;
    using Functor = std::function<T (T, T)>;

    Generator2() = default;
    Generator2(const Generator2& other) : m_functor(other.m_functor) { }
    Generator2(Generator2&& other) noexcept : m_functor(std::move(other.m_functor)) {}

    explicit Generator2(const Functor& functor) noexcept : m_functor(functor) { }
    explicit Generator2(Functor&& functor) noexcept : m_functor(std::move(functor)) { }

    template <typename U, std::enable_if_t<GeneratorCompatibleType<U>, int> = 0>
        explicit Generator2(U value) : m_functor([v = static_cast<T>(value)](T, T) -> T { return v; }) { }

    template <typename U, std::enable_if_t<meta::IsArrayLikeV<U>, int> = 0>
        explicit Generator2(const U& value) : Generator2(value[0]) { }

    Generator2& operator=(const Generator2& other) { m_functor = other.m_functor; return *this; }
    Generator2& operator=(Generator2&& other) noexcept { m_functor = std::move(other.m_functor); return *this; }

    T operator()(T x, T y) const { return m_functor(x, y); }

private:
    Functor m_functor = [](T, T) { return 0; };
};

template <typename T, typename Enable = std::enable_if_t<GeneratorCompatibleType<T>>>
    class Generator3 {
public:
    using Type = T;
    using Functor = std::function<T (T, T, T)>;

    Generator3() = default;
    Generator3(const Generator3& other) : m_functor(other.m_functor) { }
    Generator3(Generator3&& other) noexcept : m_functor(std::move(other.m_functor)) {}

    explicit Generator3(const Functor& functor) noexcept : m_functor(functor) { }
    explicit Generator3(Functor&& functor) noexcept : m_functor(std::move(functor)) { }

    template <typename U, std::enable_if_t<GeneratorCompatibleType<U>, int> = 0>
        explicit Generator3(U value) : m_functor([v = static_cast<T>(value)](T, T, T) -> T { return v; }) { }

    template <typename U, std::enable_if_t<meta::IsArrayLikeV<U>, int> = 0>
        explicit Generator3(const U& value) : Generator3(value[0]) { }

    Generator3& operator=(const Generator3& other) { m_functor = other.m_functor; return *this; }
    Generator3& operator=(Generator3&& other) noexcept { m_functor = std::move(other.m_functor); return *this; }

    T operator()(T x, T y, T z) const { return m_functor(x, y, z); }

private:
    Functor m_functor = [](T, T, T) { return 0; };
};

}
