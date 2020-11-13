#pragma once

#include <limits>
#include <vector>
#include <charconv>
#include <type_traits>

#include "fmt/fmt.h"
#include "cpgf/variant.h"
#include "core/common/exception.h"
#include "middleware/gscheme/meta/gs_type_interface.h"


namespace gs {

template<typename T, typename ItemType = std::remove_cvref_t<decltype(std::declval<T>()[0])>, typename Enable = std::enable_if_t<std::is_integral_v<ItemType> || std::is_floating_point_v<ItemType>>>
class CompositeType final : public ICompositeType {
public:
    using FieldType = ItemType;

    struct CompositeTypeItem {
        ptrdiff_t index;
        std::string name;
        IPrimitiveTypeEdit* primitiveType;
    };

public:
    CompositeType() = delete;
    CompositeType(std::vector<CompositeTypeItem> properties)
        : m_properties(properties) {

    }

    ~CompositeType() final {
        for(auto& property: m_properties) {
            delete property.primitiveType;
        }
    }

public:
    void DisableUI() {
        m_enabledUI = false;
    }

public:
    bool IsEnabledUI() const noexcept final {
        return m_enabledUI;
    }

    std::type_index GetTypeIndex() const {
        return std::type_index(typeid(T));
    }

    bool IsChanged() const final {
        for(const auto& property: m_properties) {
            if (property.primitiveType->IsChanged()) {
                return true;
            }
        }

        return false;
    }

    void SetValue(const cpgf::GVariant& value) final {
        T tmp = cpgf::fromVariant<T>(value);
        for(auto& property: m_properties) {
            property.primitiveType->SetValue(cpgf::createVariant<ItemType>(tmp[property.index], true));
        }
    }

    cpgf::GVariant GetValue() const final {
        T tmp;
        for(const auto& property: m_properties) {
            tmp[property.index] = cpgf::fromVariant<ItemType>(property.primitiveType->GetValue());
        }

        return cpgf::createVariant<T>(tmp, true);
    }

    size_t CountItem() const final {
        return m_properties.size();
    }

    std::string_view GetItemName(size_t index) const final {
        if (index >= m_properties.size()) {
            throw EngineError("gs::CompositeType::GetItemName: index {} is not valid", index);
        }

        return m_properties[index].name;
    }

    IPrimitiveType* GetItemValue(size_t index) const final {
        if (index >= m_properties.size()) {
            throw EngineError("gs::CompositeType::GetItemValue: index {} is not valid", index);
        }

        return static_cast<IPrimitiveType*>(m_properties[index].primitiveType);
    }

private:
    bool m_enabledUI = true;
    std::vector<CompositeTypeItem> m_properties;
};

}
