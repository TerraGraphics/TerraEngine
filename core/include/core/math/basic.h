#pragma once

#include "core/dg/dg.h"
#include "core/dg/math.h"


using UInt2 = dg::Vector2<uint32_t>;

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

    union {
        float value[4] = {0, 0, 0, 1.0f};
        struct {
            float red;
            float green;
            float blue;
            float alpha;
        };
    };
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

    union {
        uint32_t value = 0xFF000000;
        struct {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t alpha;
        };
    };
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

    union {
        float value[3] = {0, 0, 0};
        struct {
            float red;
            float green;
            float blue;
        };
    };
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

    union {
        uint32_t value = 0;
        struct {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t _;
        };
    };
};
