#pragma once

#include <limits>
#include <vector>
#include <charconv>
#include <type_traits>

#include "fmt/fmt.h"
#include "cpgf/variant.h"
#include "core/common/exception.h"
#include "middleware/gscheme/meta/gs_meta_type_interface.h"


namespace gs {

template<typename T, typename ItemType = std::remove_cvref_t<decltype(T[0])>, typename Enable = std::enable_if_t<std::is_integral_v<ItemType> || std::is_floating_point_v<ItemType>>>
class MetaCompositeType : final public IMetaCompositeType {
public:
    struct MetaCompositeTypeItem {
        size_t index;
        std::string name;
        IMetaPrimitiveTypeEdit* primitiveType;
    };
public:
    MetaCompositeType() = delete;
    MetaCompositeType(std::vector<MetaCompositeTypeItem> properties)
        : m_properties(properties) {

    }

    ~MetaCompositeType() final {
        for(auto& property: m_properties) {
            delete property.primitiveType;
        }
    }

public:
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
        T tmp = cpgf::fromVariant<T>(value);
        for(const auto& property: m_properties) {
            tmp[property.index] = property.primitiveType->GetValue();
        }

        return cpgf::createVariant<T>(tmp, true);
    }

    size_t CountItem() const final {
        return m_properties.size();
    }

    std::string_view GetItemName(size_t index) const final {
        if (index >= m_properties.size()) {
            throw EngineError("gs::MetaCompositeType::GetItemName: index {} is not valid", index);
        }

        return m_properties[index].name;
    }

    IMetaPrimitiveType* GetItemValue(size_t index) const final {
        if (index >= m_properties.size()) {
            throw EngineError("gs::MetaCompositeType::GetItemValue: index {} is not valid", index);
        }

        return static_cast<IMetaPrimitiveType*>(m_properties[index].primitiveType);
    }

private:
    std::vector<MetaCompositeTypeItem> m_properties;
};

}
