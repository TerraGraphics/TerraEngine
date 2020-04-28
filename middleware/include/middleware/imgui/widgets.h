#pragma once

#include <variant>
#include "core/dg/dg.h"
#include "core/math/types.h"


namespace gui {
namespace detail {

using NumberVariant = std::variant<int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double>;

bool InputScalar(const char* label, NumberVariant& value, const NumberVariant& step, const char* format);
bool InputScalarN(const char* label, NumberVariant* values, size_t components, const NumberVariant& step, const char* format);
bool Combo(const char* label, size_t& currentIndex, const char* const* itemNames, const size_t numberItems);

} // end namespace gui::detail

enum class IconType : uint8_t {
    Flow,
    Circle,
    Square,
    Grid,
    RoundSquare,
    Diamond
};

template<typename T> struct Range {
    Range(const T& minValue, const T& maxValue) : minValue(minValue), maxValue(maxValue) {}
    bool Check(const T& value) const noexcept { return ((value >= minValue) && (value <= maxValue)); }
    bool Check(const detail::NumberVariant* values, size_t valuesNumber) const noexcept {
        for (size_t i=0; i!=valuesNumber; ++i) {
            if (!Check(std::get<T>(values[i]))) {
                return false;
            }
        }

        return true;
    }

    const T& minValue;
    const T& maxValue;
};


template<typename T>
    bool InputScalar(const char* label, T& value, const T step, const Range<T>& range, const char* format = nullptr) {
        detail::NumberVariant tmpValue = value;
        if (detail::InputScalar(label, tmpValue, step, format)) {
            if (range.Check(std::get<T>(tmpValue))) {
                value = std::get<T>(tmpValue);
                return true;
            }
        }

        return false;
    }

template<typename T>
    bool InputScalar(const char* label, T& value, const T step, const char* format = nullptr) {
        constexpr const T minValue = std::numeric_limits<T>::lowest();
        constexpr const T maxValue = std::numeric_limits<T>::max();
        return InputScalar(label, value, step, Range<T>(minValue, maxValue), format);
    }

template<typename T, size_t N>
    bool InputScalarN(const char* label, T (&values)[N], const T step, const Range<T>& range, const char* format = nullptr) {
        detail::NumberVariant tmpValues[N];
        std::copy(std::begin(values), std::end(values), std::begin(tmpValues));

        if (detail::InputScalarN(label, tmpValues, N, step, format)) {
            if (range.Check(tmpValues, N)) {
                for (size_t i=0; i!=N; ++i) {
                    values[i] = std::get<T>(tmpValues[i]);
                }
                return true;
            }
        }

        return false;
    }

template<typename T, size_t N>
    bool InputScalarN(const char* label, T (&values)[N], const T step, const char* format = nullptr) {
        constexpr const T minValue = std::numeric_limits<T>::lowest();
        constexpr const T maxValue = std::numeric_limits<T>::max();
        return InputScalarN<T, N>(label, values, step, Range<T>(minValue, maxValue), format);
    }

template<typename T, size_t N>
    bool Combo(const char* label, T& value, const std::array<const char*, N>& itemNames) {
        size_t currentIndex = static_cast<size_t>(value);
        if (detail::Combo(label, currentIndex, itemNames.begin(), itemNames.size())) {
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

} // end namespace gui