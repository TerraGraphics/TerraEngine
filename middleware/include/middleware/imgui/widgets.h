#pragma once

#include "core/dg/dg.h"
#include "core/math/types.h"


namespace detail {

enum class DataType : uint8_t {
    S8,       // int8_t
    U8,       // uint8_t
    S16,      // int16_t
    U16,      // uint16_t
    S32,      // int32_t
    U32,      // uint32_t
    S64,      // int64_t
    U64,      // uint64_t
    Float,    // float
    Double,   // double
    Unknown
};

template<typename T> constexpr DataType TypeID() { return DataType::Unknown; }
template<> constexpr DataType TypeID<int8_t>() { return DataType::S8; }
template<> constexpr DataType TypeID<uint8_t>() { return DataType::U8; }
template<> constexpr DataType TypeID<int16_t>() { return DataType::S16; }
template<> constexpr DataType TypeID<uint16_t>() { return DataType::U16; }
template<> constexpr DataType TypeID<int32_t>() { return DataType::S32; }
template<> constexpr DataType TypeID<uint32_t>() { return DataType::U32; }
template<> constexpr DataType TypeID<int64_t>() { return DataType::S64; }
template<> constexpr DataType TypeID<uint64_t>() { return DataType::U64; }
template<> constexpr DataType TypeID<float>() { return DataType::Float; }
template<> constexpr DataType TypeID<double>() { return DataType::Double; }

template <typename T>
    struct IsSupportedType {
        using value = typename std::enable_if<
        std::is_same_v<int8_t, T> ||
        std::is_same_v<uint8_t, T> ||
        std::is_same_v<int16_t, T> ||
        std::is_same_v<uint16_t, T> ||
        std::is_same_v<int32_t, T> ||
        std::is_same_v<uint32_t, T> ||
        std::is_same_v<int64_t, T> ||
        std::is_same_v<uint64_t, T> ||
        std::is_same_v<float, T> ||
        std::is_same_v<double, T>
        >::type;
    };

bool InputScalar(const char* label, DataType dataType, void* value, const void* step, const void* stepFast, const char* format);
bool Combo(const char* label, size_t& currentIndex, const char* const* itemNames, const size_t numberItems);
}

enum class IconType : uint8_t {
    Flow,
    Circle,
    Square,
    Grid,
    RoundSquare,
    Diamond
};

template<typename T> struct Step {
    Step(const T& normal, const T& fast) : normal(normal), fast(fast) {}

    const T& normal;
    const T& fast;
};

template<typename T> struct Range {
    Range(const T& minValue, const T& maxValue) : minValue(minValue), maxValue(maxValue) {}
    bool Check(const T& value) const noexcept { return ((value >= minValue) && (value <= maxValue)); }

    const T& minValue;
    const T& maxValue;
};

template<typename T, typename = typename detail::IsSupportedType<T>::value>
    bool InputScalar(const char* label, T& value, const Step<T>& step, const Range<T>& range, const char* format = nullptr) {
        T tmpValue = value;
        if (detail::InputScalar(label, detail::TypeID<T>(), &tmpValue, &step.normal, &step.fast, format)) {
            if (range.Check(tmpValue)) {
                value = tmpValue;
                return true;
            }
        }

        return false;
    }

template<typename T>
    bool InputScalar(const char* label, T& value, const Step<T>& step, const char* format = nullptr) {
        static constexpr const T minValue = std::numeric_limits<T>::lowest();
        static constexpr const T maxValue = std::numeric_limits<T>::max();
        return InputScalar(label, value, step, Range<T>(minValue, maxValue), format);
    }

template<typename T>
    bool InputScalar(const char* label, T& value, const T normalStep, const char* format = nullptr) {
        const T fastStep = normalStep * static_cast<T>(10);
        return InputScalar(label, value, Step<T>(normalStep, fastStep), format);
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
