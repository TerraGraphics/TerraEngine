#pragma once

#include <limits>
#include <type_traits>


namespace gs {

template<typename T, typename Enable = std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>>>
struct PrimitiveTypeProperty {
    using TCheckFunc = bool(*)(T);

public:
    void Max(T maxValue) {
        m_maxValue = maxValue;
    }

    void Min(T minValue) {
        m_minValue = minValue;
    }

    void CheckFunc(const TCheckFunc func) {
        m_checkFunc = func;
    }

public:
    T m_maxValue = std::numeric_limits<T>::max();
    T m_minValue = std::numeric_limits<T>::min();
    TCheckFunc m_checkFunc = nullptr;
};

}
