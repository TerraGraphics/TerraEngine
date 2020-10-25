#pragma once

#include <array>
#include <limits>
#include <variant>
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <string_view>


namespace gui {
namespace detail {

using NumberVariant = std::variant<int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double>;

bool InputScalar(const char* label, NumberVariant& value, const NumberVariant& step, const NumberVariant& minValue, const NumberVariant& maxValue, const char* format);
bool InputScalarN(const char* label, NumberVariant* values, size_t components, const NumberVariant& step, const NumberVariant& minValue, const NumberVariant& maxValue, const char* format);
bool Combo(const char* label, size_t& currentIndex, const char* const* itemNames, const size_t numberItems);

} // end namespace gui::detail

template<typename T> struct Range {
    Range(T minValue = std::numeric_limits<T>::lowest(), T maxValue = std::numeric_limits<T>::max())
        : minValue(minValue)
        , maxValue(maxValue) {}


    const T minValue;
    const T maxValue;
};

template<typename T>
    bool InputScalar(const char* label, T& value, const T step, const Range<T> range, const char* format = nullptr) {
        detail::NumberVariant tmpValue = value;
        if (detail::InputScalar(label, tmpValue, step, range.minValue, range.maxValue, format)) {
            value = std::get<T>(tmpValue);
            return true;
        }

        return false;
    }

template<typename T>
    bool InputScalar(const char* label, T& value, const T step, const char* format = nullptr) {
        return InputScalar(label, value, step, Range<T>(), format);
    }

template<typename T, size_t N>
    bool InputScalarN(const char* label, T (&values)[N], const T step, const Range<T> range, const char* format = nullptr) {
        detail::NumberVariant tmpValues[N];
        std::copy(std::begin(values), std::end(values), std::begin(tmpValues));

        if (detail::InputScalarN(label, tmpValues, N, step, range.minValue, range.maxValue, format)) {
            for (size_t i=0; i!=N; ++i) {
                values[i] = std::get<T>(tmpValues[i]);
            }
            return true;
        }

        return false;
    }

template<typename T, size_t N>
    bool InputScalarN(const char* label, T (&values)[N], const T step, const char* format = nullptr) {
        return InputScalarN(label, values, step, Range<T>(), format);
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

void Text(std::string_view text);

} // end namespace gui
