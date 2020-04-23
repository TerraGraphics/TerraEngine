#pragma once

#include "core/dg/dg.h"
#include "core/math/types.h"


namespace detail {
    bool Combo(const char* label, size_t& currentIndex, const char** itemNames, const size_t numberItems);
}

enum class IconType : uint8_t {
    Flow,
    Circle,
    Square,
    Grid,
    RoundSquare,
    Diamond
};

template<typename T>
    bool Combo(const char* label, T& value, const char** itemNames, const T numberItems) {
        size_t currentIndex = static_cast<size_t>(value);
        if (detail::Combo(label, currentIndex, itemNames, static_cast<size_t>(numberItems))) {
            value = static_cast<T>(currentIndex);
            return true;
        }

        return false;
    }

math::Rect Image(TextureViewRaw texture, math::Size size, bool isOpenGL,
    math::PointF uv0 = math::PointF(0, 0), math::PointF uv1 = math::PointF(1, 1),
    math::Color tintCol = math::Color(255));


void NodeIcon(const math::Size& size, IconType type, bool filled,
    math::Color color = math::Color(255), math::Color innerColor = math::Color(0));
