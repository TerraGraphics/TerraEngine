#pragma once

#include <limits>
#include <charconv>
#include <type_traits>

#include "fmt/fmt.h"
#include "cpgf/variant.h"
#include "middleware/gscheme/meta/gs_type_interface.h"


namespace gs {

template<typename T, typename Enable = std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>>>
class PrimitiveType final : public IPrimitiveTypeEdit {
public:
    using TLimitFunc = T(*)(T);

public:
    PrimitiveType() = default;
    ~PrimitiveType() final = default;

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

    void LimitFunc(const TLimitFunc func) {
        m_limitFunc = func;
    }

public:
    bool IsChanged() const final {
        return m_isChanged;
    }

    void SetValue(const cpgf::GVariant& value) final {
        ApplyLimitsAndSet(cpgf::fromVariant<T>(value));
        m_isChanged = false;
    }

    cpgf::GVariant GetValue() const final {
        return cpgf::createVariant<T>(m_value, true);
    }

public:
    std::string ToString() const final {
        if constexpr (std::is_floating_point_v<T>) {
            int maxExp = 10;
            if constexpr (std::is_same_v<T, float>) {
                maxExp = 7;
            }

            uint8_t precision = 0;
            auto absValue = std::abs(static_cast<double>(m_value));
            for (int i=maxExp; i>0; i--) {
                if (absValue >= std::pow(10., static_cast<double>(i))) {
                    break;
                }
                ++precision;
            }

            precision = std::min(precision, m_maxPrecision);
            return fmt::format("{:." + std::to_string(precision) + "f}", m_value);
        }

        return std::to_string(m_value);
    }

    void FromString(const std::string& value) final {
        if constexpr (std::is_same_v<T, float>) {
            try {
                std::size_t pos;
                ApplyLimitsAndSet(std::stof(value, &pos));
            } catch(const std::out_of_range&) {
                return;
            } catch(const std::invalid_argument&) {
                return;
            }
        } else if constexpr (std::is_same_v<T, double>) {
            try {
                std::size_t pos;
                ApplyLimitsAndSet(std::stod(value, &pos));
            } catch(const std::out_of_range&) {
                return;
            } catch(const std::invalid_argument&) {
                return;
            }
        } else {
            T tmp;
            if(auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), tmp); ec == std::errc()) {
                ApplyLimitsAndSet(tmp);
            }
        }
    }

private:
    void ApplyLimitsAndSet(T value) {
        m_isChanged = true;

        if (m_limitFunc != nullptr) {
            m_value = m_limitFunc(value);
        } else if (m_minValue > value) {
            m_value = m_minValue;
        } else if (m_maxValue < value) {
            m_value = m_maxValue;
        } else {
            m_value = value;
        }
    }

private:
    T m_value = 0;
    T m_maxValue = std::numeric_limits<T>::max();
    T m_minValue = std::numeric_limits<T>::lowest();
    bool m_isChanged = false;
    uint8_t m_maxPrecision = 4;
    TLimitFunc m_limitFunc = nullptr;
};

}
