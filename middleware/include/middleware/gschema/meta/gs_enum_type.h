#pragma once

#include <limits>
#include <vector>
#include <cstdint>
#include <charconv>
#include <stdexcept>
#include <type_traits>

#include "cpgf/variant.h"
#include "middleware/gschema/meta/gs_meta_type.h"
#include "middleware/gschema/meta/gs_type_interface.h"


namespace gs {

template<typename T, typename Enable = std::enable_if_t<std::is_enum_v<T>>>
class EnumType final : public IPrimitiveTypeEdit {
    enum StateFlags : uint8_t {
        VisibleInGUI = 1,
        ValueChanged = 2,
    };

public:
    EnumType() = delete;
    EnumType(const MetaEnum* metaEnum)
        : m_value(metaEnum->GetFields()[0].value)
        , m_metaEnum(metaEnum)
        , m_items(metaEnum->GetDisplayNames()) {

    }
    ~EnumType() final = default;

public:
    void SetDisplayName(std::string_view displayName) {
        m_displayName = displayName;
    }

    void HideInGUI() {
        m_state &= static_cast<uint8_t>(~StateFlags::VisibleInGUI);
    }

public:
    bool IsVisibleInGUI() const noexcept final {
        return (m_state & StateFlags::VisibleInGUI) != 0;
    }

    bool IsShowStepButtons() const noexcept final {
        return false;
    }

    std::type_index GetTypeIndex() const {
        return std::type_index(typeid(T));
    }

    bool IsChanged() const final {
        return (m_state & StateFlags::ValueChanged) != 0;
    }

    void SetValue(const cpgf::GVariant& value) final {
        m_value = static_cast<MetaEnum::ValueType>(cpgf::fromVariant<T>(value));
        m_state &= static_cast<uint8_t>(~StateFlags::ValueChanged);
    }

    cpgf::GVariant GetValue() const final {
        return cpgf::createVariant<T>(static_cast<T>(m_value), true);
    }

public:
    bool IsEnumType() const noexcept final {
        return true;
    }

    bool IsIntegerType() const noexcept final {
        return false;
    }

    bool IsFloatingType() const noexcept final {
        return false;
    }

    std::string_view GetDisplayName() const final {
        return m_displayName;
    }

    const std::vector<std::string>& GetItems() const final {
        return m_items;
    }

    void Inc() final {
    }

    void Dec() final {
    }

    std::string ToString() const final {
        return m_metaEnum->GetDisplayNameByValue(m_value);
    }

    void FromString(const std::string& value) final {
        m_value = m_metaEnum->GetValueByDisplayName(value);
        m_state |= StateFlags::ValueChanged;
    }

private:
    MetaEnum::ValueType m_value = 0;

    uint8_t m_state = StateFlags::VisibleInGUI;
    const MetaEnum* m_metaEnum = nullptr;
    const std::vector<std::string>& m_items;
    std::string m_displayName;
};

}
