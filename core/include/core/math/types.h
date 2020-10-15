#pragma once

#include <limits>
#include <cstdint>
#include <cstdlib>

#include "dg/math.h"


namespace Diligent {

template<typename U, typename T> inline dg::Vector2<U> ToVector2(const dg::Vector2<T>& value) {
    return dg::Vector2<U>(static_cast<U>(value.x), static_cast<U>(value.y));
}

template<typename U, typename T> inline dg::Vector3<U> ToVector3(const dg::Vector3<T>& value) {
    return dg::Vector3<U>(static_cast<U>(value.x), static_cast<U>(value.y), static_cast<U>(value.z));
}

template<typename U, typename T> inline dg::Vector4<U> ToVector4(const dg::Vector4<T>& value) {
    return dg::Vector4<U>(static_cast<U>(value.x), static_cast<U>(value.y), static_cast<U>(value.z), static_cast<U>(value.w));
}

template<typename U, typename T> inline dg::Matrix4x4<U> ToMatrix4x4(const dg::Matrix4x4<T>& value) {
    return dg::Matrix4x4<U>(
                static_cast<U>(value._11), static_cast<U>(value._12), static_cast<U>(value._13), static_cast<U>(value._14),
                static_cast<U>(value._21), static_cast<U>(value._22), static_cast<U>(value._23), static_cast<U>(value._24),
                static_cast<U>(value._31), static_cast<U>(value._32), static_cast<U>(value._33), static_cast<U>(value._34),
                static_cast<U>(value._41), static_cast<U>(value._42), static_cast<U>(value._43), static_cast<U>(value._44));
}

}

namespace math {

enum class Axis : uint8_t {
    X = 0,
    Y = 1,
    Z = 2,
};

using Axis2 = dg::Vector2<Axis>;
using Axis3 = dg::Vector3<Axis>;

Axis Next(Axis value) noexcept;
Axis Prev(Axis value) noexcept;
Axis GetThirdAxis(Axis first, Axis second) noexcept;
Axis GetThirdAxis(Axis2 values) noexcept;

enum class Direction : uint8_t {
    POS_X = 0,
    POS_Y = 1,
    POS_Z = 2,
    NEG_X = 3,
    NEG_Y = 4,
    NEG_Z = 5,
};

Axis GetAxis(Direction value) noexcept;
Direction GetDirection(Axis value) noexcept;

// For POX_ return +1, for NEG_ return -1
int8_t GetSign(Direction value) noexcept;

template <typename T> bool IsEqual(T a, T b) {
    if constexpr (std::is_integral_v<T>) {
        return (a == b);
    } else if constexpr (std::is_floating_point_v<T>) {
        return (std::abs(a - b) <= std::numeric_limits<T>::epsilon());
    } else {
        return (a == b);
    }
}

struct Color4f {
    constexpr Color4f() noexcept = default;
    constexpr Color4f(const Color4f& o) noexcept : red(o.red), green(o.green), blue(o.blue), alpha(o.alpha) {}
    constexpr Color4f(Color4f&& o) noexcept : red(std::move(o.red)), green(std::move(o.green)), blue(std::move(o.blue)), alpha(std::move(o.alpha)) {}
    constexpr explicit Color4f(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept
        : red(static_cast<float>(r) / 255.0f)
        , green(static_cast<float>(g) / 255.0f)
        , blue(static_cast<float>(b) / 255.0f)
        , alpha(static_cast<float>(a) / 255.0f) {}
    constexpr explicit Color4f(float r, float g, float b, float a = 1.0f) noexcept : red(r), green(g), blue(b), alpha(a) {}
    constexpr explicit Color4f(float v) noexcept : Color4f(v, v, v, v) {}
    constexpr explicit Color4f(uint8_t v) noexcept : Color4f(v, v, v, v) {}

    Color4f& operator=(Color4f o) noexcept {
        std::swap(red, o.red);
        std::swap(green, o.green);
        std::swap(blue, o.blue);
        std::swap(alpha, o.alpha);
        return *this;
    }

    bool operator==(const Color4f& o) const noexcept { return (IsEqual(red, o.red) && IsEqual(green, o.green) && IsEqual(blue, o.blue) && IsEqual(alpha, o.alpha) ); }
    bool operator!=(const Color4f& o) const noexcept { return (!operator==(o)); }

    dg::float4 ToFloat4() const noexcept {
        return dg::float4(red, green, blue, alpha);
    }

#pragma GCC diagnostic push
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
    union {
        float value[4] = {0, 0, 0, 1.0f};
        struct {
            float red;
            float green;
            float blue;
            float alpha;
        };
    };
#pragma GCC diagnostic pop

};

struct Color4 {
    constexpr Color4() noexcept = default;
    constexpr Color4(const Color4& o) noexcept : red(o.red), green(o.green), blue(o.blue), alpha(o.alpha) {}
    constexpr Color4(Color4&& o) noexcept : red(std::move(o.red)), green(std::move(o.green)), blue(std::move(o.blue)), alpha(std::move(o.alpha)) {}
    constexpr explicit Color4(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept : red(r), green(g), blue(b), alpha(a) {}
    constexpr explicit Color4(uint8_t v) noexcept : Color4(v, v, v, v) {}

    Color4& operator=(Color4 o) noexcept {
        std::swap(red, o.red);
        std::swap(green, o.green);
        std::swap(blue, o.blue);
        std::swap(alpha, o.alpha);
        return *this;
    }

    bool operator==(const Color4& o) const noexcept { return ((red == o.red) && (green == o.green) && (blue == o.blue) && (alpha == o.alpha)); }
    bool operator!=(const Color4& o) const noexcept { return (!operator==(o)); }

    Color4f ToColor4f() const noexcept {
        return Color4f(red, green, blue, alpha);
    }

    dg::float4 ToFloat4() const noexcept {
        return dg::float4(
            static_cast<float>(red) / 255.0f,
            static_cast<float>(green) / 255.0f,
            static_cast<float>(blue) / 255.0f,
            static_cast<float>(alpha) / 255.0f);
    }

#pragma GCC diagnostic push
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
    union {
        uint32_t value = 0xFF000000;
        struct {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t alpha;
        };
    };
#pragma GCC diagnostic pop

};

using Color = Color4;

struct Color3f {
    constexpr Color3f() noexcept = default;
    constexpr Color3f(const Color3f& o) noexcept : red(o.red), green(o.green), blue(o.blue) {}
    constexpr Color3f(Color3f&& o) noexcept : red(std::move(o.red)), green(std::move(o.green)), blue(std::move(o.blue)) {}
    constexpr explicit Color3f(uint8_t r, uint8_t g, uint8_t b) noexcept
        : red(static_cast<float>(r) / 255.0f)
        , green(static_cast<float>(g) / 255.0f)
        , blue(static_cast<float>(b) / 255.0f) {}
    constexpr explicit Color3f(float r, float g, float b) noexcept : red(r), green(g), blue(b) {}
    constexpr explicit Color3f(float v) noexcept : Color3f(v, v, v) {}
    constexpr explicit Color3f(uint8_t v) noexcept : Color3f(v, v, v) {}

    Color3f& operator=(Color3f o) noexcept {
        std::swap(red, o.red);
        std::swap(green, o.green);
        std::swap(blue, o.blue);
        return *this;
    }

    bool operator==(const Color3f& o) const noexcept { return (IsEqual(red, o.red) && IsEqual(green, o.green) && IsEqual(blue, o.blue) ); }
    bool operator!=(const Color3f& o) const noexcept { return (!operator==(o)); }

    dg::float3 ToFloat3() const noexcept {
        return dg::float3(red, green, blue);
    }

    dg::float4 ToFloat4() const noexcept {
        return dg::float4(red, green, blue, 1.f);
    }

#pragma GCC diagnostic push
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
    union {
        float value[3] = {0, 0, 0};
        struct {
            float red;
            float green;
            float blue;
        };
    };
#pragma GCC diagnostic pop
};

struct Color3 {
    constexpr Color3() noexcept = default;
    constexpr Color3(const Color3& o) noexcept : red(o.red), green(o.green), blue(o.blue) {}
    constexpr Color3(Color3&& o) noexcept : red(std::move(o.red)), green(std::move(o.green)), blue(std::move(o.blue)) {}
    constexpr explicit Color3(uint8_t r, uint8_t g, uint8_t b) noexcept : red(r), green(g), blue(b) {}
    constexpr explicit Color3(uint8_t v) noexcept : Color3(v, v, v) {}

    Color3& operator=(Color3 o) noexcept {
        std::swap(red, o.red);
        std::swap(green, o.green);
        std::swap(blue, o.blue);
        return *this;
    }

    bool operator==(const Color3& o) const noexcept { return ((red == o.red) && (green == o.green) && (blue == o.blue)); }
    bool operator!=(const Color3& o) const noexcept { return (!operator==(o)); }

    dg::float3 ToFloat3() const noexcept {
        return dg::float3(
            static_cast<float>(red) / 255.0f,
            static_cast<float>(green) / 255.0f,
            static_cast<float>(blue) / 255.0f);
    }

    dg::float4 ToFloat4() const noexcept {
        return dg::float4(
            static_cast<float>(red) / 255.0f,
            static_cast<float>(green) / 255.0f,
            static_cast<float>(blue) / 255.0f,
            1.f);
    }

#pragma GCC diagnostic push
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
    union {
        uint32_t value = 0;
        struct {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t _;
        };
    };
#pragma GCC diagnostic pop

};

// 0xAARRGGBB to 0xAABBGGRR
inline constexpr uint32_t BGRAToRGBA(uint32_t v) noexcept {
    return
        (v & 0xFF00FF00) |
        ((v & 0x00FF0000) >> 16) |
        ((v & 0x000000FF) << 16);
}

template <typename T, typename Enable = std::enable_if_t<std::is_arithmetic_v<T>>>
struct PointT {
    constexpr PointT() noexcept = default;
    constexpr PointT(const PointT& o) noexcept : x(o.x), y(o.y) {}
    constexpr PointT(PointT&& o) noexcept : x(std::move(o.x)), y(std::move(o.y)) {}
    constexpr explicit PointT(T x, T y) noexcept : x(x), y(y) {}

    PointT& operator=(PointT o) noexcept {
        std::swap(x, o.x);
        std::swap(y, o.y);
        return *this;
    }

    bool operator==(const PointT& o) const noexcept { return (IsEqual(x, o.x) && IsEqual(y, o.y)); }
    bool operator!=(const PointT& o) const noexcept { return (!operator==(o)); }

#pragma GCC diagnostic push
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
    union {
        T values[2] = {0, 0};
        struct {
            T x;
            T y;
        };
    };
#pragma GCC diagnostic pop
};

using Point = PointT<uint32_t>;
using PointU = PointT<uint32_t>;
using PointI = PointT<int32_t>;
using PointF = PointT<float>;
using PointD = PointT<double>;

template <typename T, typename Enable = std::enable_if_t<std::is_arithmetic_v<T>>>
struct SizeT {
    constexpr SizeT() noexcept = default;
    constexpr SizeT(const SizeT& o) noexcept : w(o.w), h(o.h) {}
    constexpr SizeT(SizeT&& o) noexcept : w(std::move(o.w)), h(std::move(o.h)) {}
    constexpr explicit SizeT(T w, T h) noexcept : w(w), h(h) {}

    SizeT& operator=(SizeT o) noexcept {
        std::swap(w, o.w);
        std::swap(h, o.h);
        return *this;
    }

    bool operator==(const SizeT& o) const noexcept { return (IsEqual(w, o.w) && IsEqual(h, o.h)); }
    bool operator!=(const SizeT& o) const noexcept { return (!operator==(o)); }

    SizeT operator+(const SizeT& o) const noexcept { return SizeT(w + o.w, h + o.h); }
    SizeT& operator+=(const SizeT& o) noexcept {
        w += o.w;
        h += o.h;
        return *this;
    }

#pragma GCC diagnostic push
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
    union {
        T values[2] = {0, 0};
        struct {
            T w;
            T h;
        };
    };
#pragma GCC diagnostic pop
};

using Size = SizeT<uint32_t>;
using SizeU = SizeT<uint32_t>;
using SizeI = SizeT<int32_t>;
using SizeF = SizeT<float>;
using SizeD = SizeT<double>;

template <typename T, typename Enable = std::enable_if_t<std::is_arithmetic_v<T>>>
struct RectOffsetT {
    constexpr RectOffsetT() noexcept = default;
    constexpr RectOffsetT(const RectOffsetT& o) noexcept : left(o.left), right(o.right), top(o.top), bottom(o.bottom) {}
    constexpr RectOffsetT(RectOffsetT&& o) noexcept : left(std::move(o.left)), right(std::move(o.right)), top(std::move(o.top)), bottom(std::move(o.bottom)) {}
    constexpr explicit RectOffsetT(T left, T right, T top, T bottom) noexcept : left(left), right(right), top(top), bottom(bottom) {}

    RectOffsetT& operator=(RectOffsetT o) noexcept {
        std::swap(left, o.left);
        std::swap(right, o.right);
        std::swap(top, o.top);
        std::swap(bottom, o.bottom);
        return *this;
    }

    bool operator==(const RectOffsetT& o) const noexcept { return (IsEqual(left, o.left) && IsEqual(right, o.right) && IsEqual(top, o.top) && IsEqual(bottom, o.bottom)); }
    bool operator!=(const RectOffsetT& o) const noexcept { return (!operator==(o)); }

    T Horizontal() const noexcept { return left + right; }
    T Vertical() const noexcept { return top + bottom; }
    SizeT<T> Size() const noexcept { return SizeT<T>(Horizontal(), Vertical()); }

    T left = 0;
    T right = 0;
    T top = 0;
    T bottom = 0;
};

using RectOffset = RectOffsetT<uint32_t>;
using RectOffsetU = RectOffsetT<uint32_t>;
using RectOffsetI = RectOffsetT<int32_t>;
using RectOffsetF = RectOffsetT<float>;
using RectOffsetD = RectOffsetT<double>;

template <typename T, typename Enable = std::enable_if_t<std::is_arithmetic_v<T>>>
struct RectT {
    constexpr RectT() noexcept = default;
    constexpr RectT(const RectT& o) noexcept : x(o.x), y(o.y), w(o.w), h(o.h) {}
    constexpr RectT(RectT&& o) noexcept : x(std::move(o.x)), y(std::move(o.y)), w(std::move(o.w)), h(std::move(o.h)) {}
    constexpr explicit RectT(T x, T y, T w, T h) noexcept : x(x), y(y), w(w), h(h) {}
    constexpr explicit RectT(const PointT<T>& pos, const SizeT<T>& size) noexcept : x(pos.x), y(pos.y), w(size.w), h(size.h) {}
    constexpr explicit RectT(const PointT<T>& posMin, const PointT<T>& posMax) noexcept : x(posMin.x), y(posMin.y), w(posMax.x - posMin.x), h(posMax.y - posMin.y) {}

    RectT& operator=(RectT o) noexcept {
        std::swap(x, o.x);
        std::swap(y, o.y);
        std::swap(w, o.w);
        std::swap(h, o.h);
        return *this;
    }

    bool operator==(const RectT& o) const noexcept { return (IsEqual(x, o.x) && IsEqual(y, o.y) && IsEqual(w, o.w) && IsEqual(w, o.w)); }
    bool operator!=(const RectT& o) const noexcept { return (!operator==(o)); }

    RectT operator-(const RectOffsetT<T>& o) const noexcept { return RectT(x + o.left, y + o.top, w - o.Horizontal(), h - o.Vertical()); }
    RectT& operator+=(const RectOffsetT<T>& o) noexcept {
        x += o.left;
        y += o.top;
        w -= o.Horizontal();
        h -= o.Vertical();
        return *this;
    }

    T Top() const noexcept { return y; }
    T Bottom() const noexcept { return y + h; }
    T Left() const noexcept { return x; }
    T Right() const noexcept { return x + w; }

    PointT<T> Min() const noexcept { return PointT<T>(x, y); }
    PointT<T> Max() const noexcept { return PointT<T>(x + w, y + h); }

    T Width() const noexcept { return w; }
    T Height() const noexcept { return h; }
    SizeT<T> Size() const noexcept { return SizeT<T>(w, h); }
    template<typename U, typename EnableU = std::enable_if_t<std::is_arithmetic_v<U>>>
        SizeT<U> SizeCast() const noexcept { return SizeT<U>(static_cast<U>(w), static_cast<U>(h)); }

    T CenterX() const noexcept { return x + w / 2; }
    T CenterY() const noexcept { return y + h / 2; }
    PointT<T> Center() const noexcept { return PointT<T>(CenterX(), CenterY()); }

#pragma GCC diagnostic push
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
    union {
        T valuesStart[2] = {0, 0};
        struct {
            T x;
            T y;
        };
    };
    union {
        T valuesSize[2] = {0, 0};
        struct {
            T w;
            T h;
        };
    };
#pragma GCC diagnostic pop
};

using Rect = RectT<uint32_t>;
using RectU= RectT<uint32_t>;
using RectI = RectT<int32_t>;
using RectF = RectT<float>;
using RectD = RectT<double>;

template<typename T, typename Enable = std::enable_if_t<std::is_arithmetic_v<T>>>
struct PlaneT {
    constexpr PlaneT() noexcept = default;
    constexpr PlaneT(const PlaneT& o) noexcept : a(o.a), b(o.b), c(o.c), d(o.d) {}
    constexpr PlaneT(PlaneT&& o) noexcept : a(std::move(o.a)), b(std::move(o.b)), c(std::move(o.c)), d(std::move(o.d)) {}
    constexpr explicit PlaneT(T a, T b, T c, T d) noexcept : a(a), b(b), c(c), d(d) {}
    // point(x0, y0, z0), n(l, m, n)
    // l*(x-x0) + m(y-y0) + n*(z-z0) = 0
    constexpr explicit PlaneT(const dg::Vector3<T>& point, const dg::Vector3<T>& normal) noexcept
        : a(normal.x)
        , b(normal.y)
        , c(normal.z)
        , d(-normal.x*point.x - normal.y*point.y - normal.z*point.z) {

    }
    // point(x0, y0, z0), vec0(a0, b0, c0), vec1(a1, b1, c1)
    // | x-x0 y-y0 z-z0 |
    // |   a0   b0   c0 | = (x-x0) * | b0 c0 | - (y-y0) * | a0 c0 | + (z-z0) * | a0 b0 |
    // |   a1   b1   c1 |            | b1 c1 |            | a1 c1 |            | a1 b1 |
    constexpr explicit PlaneT(const dg::Vector3<T>& point, const dg::Vector3<T>& vec0, const dg::Vector3<T>& vec1) noexcept {
        auto normal = dg::cross(vec0, vec1);
        a = normal.x;
        b = normal.y;
        c = normal.z;
        d = -normal.x*point.x - normal.y*point.y - normal.z*point.z;
    }

    PlaneT& operator=(PlaneT o) noexcept {
        std::swap(a, o.a);
        std::swap(b, o.b);
        std::swap(c, o.c);
        std::swap(d, o.d);
        return *this;
    }

    bool operator==(const PlaneT& o) const noexcept { return (IsEqual(a, o.a) && IsEqual(b, o.b) && IsEqual(c, o.c) && IsEqual(d, o.d)); }
    bool operator!=(const PlaneT& o) const noexcept { return (!operator==(o)); }

    explicit operator dg::Vector3<T>() const noexcept {
        return dg::Vector3<T>(a, b, c);
    }

    explicit operator dg::Vector4<T>() const noexcept {
        return dg::Vector4<T>(a, b, c, d);
    }

    T a = T(0);
    T b = T(0);
    T c = T(0);
    T d = T(0);
};

using Plane = PlaneT<double>;
using PlaneD = PlaneT<double>;

template<typename T, typename Enable = std::enable_if_t<std::is_arithmetic_v<T>>>
struct RayT {
    constexpr RayT() noexcept = default;
    constexpr RayT(const RayT& o) noexcept : start(o.start), direction(o.direction) {}
    constexpr RayT(RayT&& o) noexcept : start(std::move(o.start)), direction(std::move(o.direction)) {}
    constexpr explicit RayT(dg::Vector3<T> start, dg::Vector3<T> direction) noexcept : start(start), direction(direction) {}

    RayT& operator=(RayT o) noexcept {
        std::swap(start, o.start);
        std::swap(direction, o.direction);
        return *this;
    }

    bool operator==(const RayT& o) const noexcept = delete;
    bool operator!=(const RayT& o) const noexcept = delete;

    RayT& operator*=(const dg::Matrix4x4<T>& right) {
        start = static_cast<dg::Vector3<T>>(dg::Vector4<T>(start, 1) * right);
        direction = dg::normalize(static_cast<dg::Vector3<T>>(dg::Vector4<T>(direction, 0) * right));

        return *this;
    }

    dg::Vector3<T> operator[](T index) const noexcept {
        return start + direction * index;
    }

    dg::Vector3<T> start = {0, 0, 0};
    dg::Vector3<T> direction = {1, 1, 1};
};

using Ray = RayT<double>;
using RayD = RayT<double>;

template<typename T, typename Enable = std::enable_if_t<std::is_arithmetic_v<T>>> struct CylinderT {
    CylinderT() = default;
    CylinderT(T radius, T height, Axis axisUp) : radius(radius), height(height), axisUp(axisUp) {}

    T radius = 1;
    // along the axisUp, values: [0; height]
    T height = 1;
    Axis axisUp = Axis::X;
};

using Cylinder = CylinderT<double>;
using CylinderD = CylinderT<double>;

template<typename T, typename Enable = std::enable_if_t<std::is_arithmetic_v<T>>>
struct TorusT {
    constexpr TorusT() noexcept = default;
    constexpr TorusT(const TorusT& o) noexcept : minorRadius(o.minorRadius), majorRadius(o.majorRadius), axisRotation(o.axisRotation) {}
    constexpr TorusT(TorusT&& o) noexcept : minorRadius(std::move(o.minorRadius)), majorRadius(std::move(o.majorRadius)), axisRotation(std::move(o.axisRotation)) {}
    constexpr explicit TorusT(T minorRadius, T majorRadius, Axis axisRotation) noexcept
        : minorRadius(minorRadius)
        , majorRadius(majorRadius)
        , axisRotation(axisRotation) {

    }

    TorusT& operator=(TorusT o) noexcept {
        std::swap(minorRadius, o.minorRadius);
        std::swap(majorRadius, o.majorRadius);
        std::swap(axisRotation, o.axisRotation);
        return *this;
    }

    bool operator==(const TorusT& o) const noexcept { return (IsEqual(minorRadius, o.minorRadius) && IsEqual(majorRadius, o.majorRadius) && IsEqual(axisRotation, o.axisRotation)); }
    bool operator!=(const TorusT& o) const noexcept { return (!operator==(o)); }


    T minorRadius = 1;
    T majorRadius = 1;
    Axis axisRotation = Axis::X;
};

using Torus = TorusT<double>;
using TorusD = TorusT<double>;

} // namespace math

namespace std {

template<typename T>
math::SizeT<T> min(math::SizeT<T> first, math::SizeT<T> second) {
    return math::SizeT<T>(
        std::min(first.w, second.w),
        std::min(first.h, second.h));
}

template<typename T>
math::SizeT<T> max(math::SizeT<T> first, math::SizeT<T> second) {
    return math::SizeT<T>(
        std::max(first.w, second.w),
        std::max(first.h, second.h));
}

} // namespace std
