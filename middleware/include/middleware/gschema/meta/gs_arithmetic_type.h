#pragma once

#include <limits>
#include <cstdint>
#include <charconv>
#include <stdexcept>
#include <type_traits>

#include "cpgf/variant.h"
#include "middleware/gschema/meta/gs_format.h"
#include "middleware/gschema/meta/gs_type_interface.h"


namespace gs {

template<typename T, typename Enable = std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>>>
class ArithmeticType final : public IPrimitiveTypeEdit {
    enum StateFlags : uint8_t {
        VisibleInGUI = 1,
        ShowStepButtons = 2,
        StepChanged = 4,
        ValueChanged = 8,
    };

public:
    using TLimitFunc = T(*)(T);
    using TStepFunc = T(*)(T, bool inc);

public:
    ArithmeticType() = default;
    ~ArithmeticType() final = default;

public:
    void Step(T value) {
        m_step = value;
        m_state |= StateFlags::StepChanged;
    }

    void Max(T value) {
        m_maxValue = value;
        m_minValue = std::min(m_minValue, m_maxValue);
        RecalcStep();
    }

    void Min(T value) {
        m_minValue = value;
        m_maxValue = std::max(m_minValue, m_maxValue);
        RecalcStep();
    }

    void Funcs(const TLimitFunc limitFunc, const TStepFunc stepFunc) {
        m_limitFunc = limitFunc;
        m_stepFunc = stepFunc;
    }

    void MaxPrecision(uint8_t value) {
        m_maxPrecision = value;
    }

    void SetDisplayName(std::string_view displayName) {
        m_displayName = displayName;
    }

    void HideInGUI() {
        m_state &= ~StateFlags::VisibleInGUI;
    }

    void HideStepButtons() {
        m_state &= ~StateFlags::ShowStepButtons;
    }

public:
    bool IsVisibleInGUI() const noexcept final {
        return (m_state & StateFlags::VisibleInGUI) != 0;
    }

    bool IsShowStepButtons() const noexcept final {
        return (m_state & StateFlags::ShowStepButtons) != 0;
    }

    std::type_index GetTypeIndex() const {
        return std::type_index(typeid(T));
    }

    bool IsChanged() const final {
        return (m_state & StateFlags::ValueChanged) != 0;
    }

    void SetValue(const cpgf::GVariant& value) final {
        ApplyLimitsAndSet(cpgf::fromVariant<T>(value));
        m_state &= ~StateFlags::ValueChanged;
    }

    cpgf::GVariant GetValue() const final {
        return cpgf::createVariant<T>(m_value, true);
    }

public:
    bool IsEnumType() const noexcept final {
        return false;
    }

    bool IsIntegerType() const noexcept final {
        return std::is_integral_v<T>;
    }

    bool IsFloatingType() const noexcept final {
        return std::is_floating_point_v<T>;
    }

    std::string_view GetDisplayName() const final {
        return m_displayName;
    }

    const std::vector<std::string>& GetItems() const final {
        static std::vector<std::string> items;
        return items;
    }

    void Inc() final {
        T newValue = m_value;
        if (m_stepFunc != nullptr) {
            newValue = m_stepFunc(m_value, true);
        } else if ((m_maxValue - m_step) <= m_value) {
            newValue = m_maxValue;
        } else {
            newValue += m_step;
        }

        ApplyLimitsAndSet(newValue);
    }

    void Dec() final {
        T newValue = m_value;
        if (m_stepFunc != nullptr) {
            newValue = m_stepFunc(m_value, false);
        } else if ((m_minValue + m_step) >= m_value) {
            newValue = m_minValue;
        } else {
            newValue -= m_step;
        }

        ApplyLimitsAndSet(newValue);
    }

    std::string ToString() const final {
        if constexpr (std::is_floating_point_v<T>) {
            int maxExp = 10;
            if constexpr (std::is_same_v<T, float>) {
                maxExp = 7;
            }

            return gs::ToString(static_cast<double>(m_value), maxExp, m_maxPrecision);
        }

        if (m_value >= 0) {
            return gs::ToString(static_cast<uint64_t>(m_value));
        }

        return gs::ToString(static_cast<int64_t>(m_value));
    }

    void FromString(const std::string& value) final {
        if constexpr (std::is_same_v<T, float>) {
            try {
                std::size_t pos;
                ApplyLimitsAndSet(std::stof(value, &pos));
            } catch(const std::out_of_range&) {
                // pass
            } catch(const std::invalid_argument&) {
                // pass
            }
        } else if constexpr (std::is_same_v<T, double>) {
            try {
                std::size_t pos;
                ApplyLimitsAndSet(std::stod(value, &pos));
            } catch(const std::out_of_range&) {
                // pass
            } catch(const std::invalid_argument&) {
                // pass
            }
        } else {
            T tmp;
            if(auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), tmp); ec == std::errc()) {
                ApplyLimitsAndSet(tmp);
            }
        }
    }

private:
    void RecalcStep() {
        if constexpr (std::is_floating_point_v<T>) {
            T step = static_cast<T>((m_maxValue - m_minValue) / static_cast<T>(1000));
            if (((m_state & StateFlags::StepChanged) == 0) && (step <= 1.f))  {
                m_step = step;
            }
        }
    }

    void ApplyLimitsAndSet(T value) {
        m_state |= StateFlags::ValueChanged;

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
    T m_step = static_cast<T>(1);
    T m_maxValue = std::numeric_limits<T>::max();
    T m_minValue = std::numeric_limits<T>::lowest();

    uint8_t m_maxPrecision = 4;
    uint8_t m_state = StateFlags::VisibleInGUI | StateFlags::ShowStepButtons;
    std::string m_displayName;
    TLimitFunc m_limitFunc = nullptr;
    TStepFunc m_stepFunc = nullptr;
};

}
