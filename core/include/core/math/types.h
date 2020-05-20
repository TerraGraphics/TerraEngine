#pragma once

#include <limits>
#include <cstdint>
#include <cstdlib>

#include "core/dg/math.h"


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

Axis Next(Axis value);
Axis Prev(Axis value);
Axis GetThirdAxis(Axis first, Axis second);
Axis GetThirdAxis(Axis2 values);

enum class Direction : uint8_t {
    POS_X = 0,
    POS_Y = 1,
    POS_Z = 2,
    NEG_X = 3,
    NEG_Y = 4,
    NEG_Z = 5,
};

Axis GetAxis(Direction value);
Direction GetDirection(Axis value);

// For POX_ return +1, for NEG_ return -1
int8_t GetSign(Direction value);

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
    Color4f() = default;
    Color4f(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept
        : red(static_cast<float>(r) / 255.0f)
        , green(static_cast<float>(g) / 255.0f)
        , blue(static_cast<float>(b) / 255.0f)
        , alpha(static_cast<float>(a) / 255.0f) {}

    Color4f(float r, float g, float b, float a = 1.0f) noexcept : red(r), green(g), blue(b), alpha(a) {}
    Color4f(float v) noexcept : Color4f(v, v, v, v) {}

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
    Color4() = default;
    Color4(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept : red(r), green(g), blue(b), alpha(a) {}
    Color4(uint8_t v) noexcept : Color4(v, v, v, v) {}

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
    Color3f() = default;
    Color3f(uint8_t r, uint8_t g, uint8_t b) noexcept
        : red(static_cast<float>(r) / 255.0f)
        , green(static_cast<float>(g) / 255.0f)
        , blue(static_cast<float>(b) / 255.0f) {}

    Color3f(float r, float g, float b) noexcept : red(r), green(g), blue(b) {}
    Color3f(float v) noexcept : Color3f(v, v, v) {}

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
    Color3() = default;
    Color3(uint8_t r, uint8_t g, uint8_t b) noexcept : red(r), green(g), blue(b) {}
    Color3(uint8_t v) noexcept : Color3(v, v, v) {}

    Color3f ToColor3f() const noexcept {
        return Color3f(red, green, blue);
    }

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
inline uint32_t BGRAToRGBA(uint32_t v) {
    return
        (v & 0xFF00FF00) |
        ((v & 0x00FF0000) >> 16) |
        ((v & 0x000000FF) << 16);
}

template <typename T>
struct PointT {
    PointT() = default;
    PointT(T x, T y) noexcept : x(x), y(y) {}

    T x = 0;
    T y = 0;
};

using Point = PointT<uint32_t>;
using PointI = PointT<int32_t>;
using PointF = PointT<float>;
using PointD = PointT<double>;

template <typename T>
struct SizeT {
    SizeT() noexcept : w(0), h(0) {}
    SizeT(T w, T h) noexcept : w(w), h(h) {}

    bool operator==(SizeT other) const noexcept { return (IsEqual(w, other.w) && IsEqual(h, other.h)); }
    bool operator!=(SizeT other) const noexcept { return (!operator==(other)); }

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
using Sizef = SizeT<float>;

template <typename T>
struct RectT {
    RectT() = default;
    RectT(const PointT<T>& posMin, const PointT<T>& posMax) noexcept : x(posMin.x), y(posMin.y), w(posMax.x - posMin.x), h(posMax.y - posMin.y) {}
    RectT(T x, T y, T w, T h) noexcept : x(x), y(y), w(w), h(h) {}

    bool operator==(RectT other) const noexcept {
        return (IsEqual(x, other.x) && IsEqual(y, other.y) && IsEqual(w, other.w) && IsEqual(h, other.h));
    }
    bool operator!=(RectT other) const noexcept { return (!operator==(other)); }

    T Top() const noexcept { return y; }
    T Bottom() const noexcept { return y + h; }
    T Left() const noexcept { return x; }
    T Right() const noexcept { return x + w; }

    PointT<T> Min() const noexcept { return PointT<T>(x, y); }
    PointT<T> Max() const noexcept { return PointT<T>(x + w, y + h); }

    T Width() const noexcept { return w; }
    T Height() const noexcept { return h; }
    SizeT<T> Size() const noexcept { return SizeT<T>(w, h); }
    template<typename U> SizeT<U> SizeCast() const noexcept { return SizeT<U>(static_cast<U>(w), static_cast<U>(h)); }

    T CenterX() const noexcept { return x + w / 2; }
    T CenterY() const noexcept { return y + h / 2; }
    PointT<T> Center() const noexcept { return PointT<T>(CenterX(), CenterY()); }

    RectT CutOffTop(T height) {
        auto result = RectT(x, y, w, height);
        y += height;
        return result;
    }

    RectT CutOffRight(T width) {
        w -= width;
        return RectT(w, y, width, h);
    }


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
using RectI = RectT<int32_t>;
using RectF = RectT<float>;
using RectD = RectT<double>;

template<typename T> struct PlaneT {
    PlaneT() = default;
    PlaneT(T a, T b, T c, T d) : a(a), b(b), c(c), d(d) {}

    // point(x0, y0, z0), n(l, m, n)
    // l*(x-x0) + m(y-y0) + n*(z-z0) = 0
    PlaneT(const dg::Vector3<T>& point, const dg::Vector3<T>& normal)
        : a(normal.x)
        , b(normal.y)
        , c(normal.z)
        , d(-normal.x*point.x - normal.y*point.y - normal.z*point.z) {

    }

    // point(x0, y0, z0), vec0(a0, b0, c0), vec1(a1, b1, c1)
    // | x-x0 y-y0 z-z0 |
    // |   a0   b0   c0 | = (x-x0) * | b0 c0 | - (y-y0) * | a0 c0 | + (z-z0) * | a0 b0 |
    // |   a1   b1   c1 |            | b1 c1 |            | a1 c1 |            | a1 b1 |
    PlaneT(const dg::Vector3<T>& point, const dg::Vector3<T>& vec0, const dg::Vector3<T>& vec1) {
        auto normal = dg::cross(vec0, vec1);
        a = normal.x;
        b = normal.y;
        c = normal.z;
        d = -normal.x*point.x - normal.y*point.y - normal.z*point.z;
    }

    operator dg::Vector3<T>() const noexcept {
        return dg::Vector3<T>(a, b, c);
    }

    operator dg::Vector4<T>() const noexcept {
        return dg::Vector4<T>(a, b, c, d);
    }

    T a = T(0);
    T b = T(0);
    T c = T(0);
    T d = T(0);
};

using Plane = PlaneT<double>;

template<typename T> struct RayT {
    RayT() = default;
    RayT(dg::Vector3<T> start, dg::Vector3<T> direction) : start(start), direction(direction) {}

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

template<typename T> struct CylinderT {
    CylinderT() = default;
    CylinderT(T radius, T height, Axis axisUp) : radius(radius), height(height), axisUp(axisUp) {}

    T radius = 1;
    // along the axisUp, values: [0; height]
    T height = 1;
    Axis axisUp = Axis::X;
};

using Cylinder = CylinderT<double>;

template<typename T> struct TorusT {
    TorusT() = default;
    TorusT(T minorRadius, T majorRadius, Axis axisRotation)
        : minorRadius(minorRadius)
        , majorRadius(majorRadius)
        , axisRotation(axisRotation) {

    }

    T minorRadius = 1;
    T majorRadius = 1;
    Axis axisRotation = Axis::X;
};

using Torus = TorusT<double>;

} // namespace math
