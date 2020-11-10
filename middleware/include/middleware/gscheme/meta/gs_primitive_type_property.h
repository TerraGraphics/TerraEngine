#pragma once

#include <limits>
#include <type_traits>


namespace gs {

template<typename T, typename Enable = std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>>>
struct PrimitiveTypeProperty {
    using TCheckFunc = bool(*)(T);

public:
    void MaxPrecision(uint8_t value) {
        m_maxPrecision = value;
    }

    void Max(T value) {
        m_maxValue = value;
    }

    void Min(T value) {
        m_minValue = value;
    }

    void CheckFunc(const TCheckFunc func) {
        m_checkFunc = func;
    }

public:
    uint8_t m_maxPrecision = 4;
    T m_maxValue = std::numeric_limits<T>::max();
    T m_minValue = std::numeric_limits<T>::lowest();
    TCheckFunc m_checkFunc = nullptr;
};

}
