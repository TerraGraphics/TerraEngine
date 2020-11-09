#pragma once

#include <limits>
#include <charconv>
#include <type_traits>

#include "fmt/fmt.h"
#include "cpgf/variant.h"
#include "core/common/exception.h"
#include "middleware/gscheme/meta/gs_type_interface.h"


namespace gs {

template<typename T, typename Enable = std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>>>
class PrimitiveType : final public IPrimitiveTypeEdit {
public:
    PrimitiveType() = delete;
    PrimitiveType(PrimitiveTypeProperty<T>* property)
        : m_property(property) {

    }

    ~PrimitiveType() final {
        delete m_property;
    }

public:
    bool IsChanged() const final {
        return m_isChanged;
    }

    void SetValue(const cpgf::GVariant& value) final {
        T tmp = cpgf::fromVariant<T>(value);
        if (!SetIsValid(tmp)) {
            throw EngineError("gs::PrimitiveType::SetValue: arg {} is not valid by PrimitiveTypeProperty", tmp);
        }
        m_isChanged = false;
    }

    cpgf::GVariant GetValue() const final {
        return cpgf::createVariant<T>(m_value, true);
    }

public:
    std::string ToString() const final {
        fmt::memory_buffer buffer;
        fmt::format_to(buffer, "{}", m_value);
        return buffer.data();
    }

    bool FromString(const std::string& value) final {
        if constexpr (std::is_same_v<T, float>) {
            try {
                std::size_t pos;
                return SetIsValid(std::stof(in, &pos));
            } catch(const std::out_of_range&) {
                return false;
            } catch(const std::invalid_argument&) {
                return false;
            }
        } else if constexpr (std::is_same_v<T, double>) {
            try {
                std::size_t pos;
                return SetIsValid(std::stod(in, &pos));
            } catch(const std::out_of_range&) {
                return false;
            } catch(const std::invalid_argument&) {
                return false;
            }
        } else {
            T tmp;
            if(auto [p, ec] = std::from_chars(in.data(), in.data() + in.size(), tmp); ec == std::errc()) {
                return SetIsValid(tmp);
            }

            return false;
        }
    }

private:
    bool SetIsValid(T value) {
        bool isValid = false;

        if (m_property->m_checkFunc != nullptr) {
            isValid = m_property->m_checkFunc(value);
            if (isValid) {
                m_value = value;
                m_isChanged = true;
            }

            return isValid;
        }

        isValid = ((m_minValue <= value) && (value <= m_maxValue));
        if (isValid) {
            m_value = value;
            m_isChanged = true;
        }

        return isValid;
    }

private:
    T m_value = 0;
    bool m_isChanged = false;
    PrimitiveTypeProperty<T>* m_property = nullptr;
};

}
