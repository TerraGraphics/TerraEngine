#pragma once

#include <vector>
#include <cstddef>
#include <typeindex>
#include <type_traits>

#include "cpgf/variant.h"


namespace gs {

struct IPrimitiveType;
struct IPrimitiveTypeEdit;
class CompositeType {
public:
    struct CompositeTypeItem {
        ptrdiff_t index;
        IPrimitiveTypeEdit* primitiveType;
    };

public:
    CompositeType() = delete;
    CompositeType(std::type_index typeIndex, const std::vector<CompositeTypeItem>& properties);
    virtual ~CompositeType();

public:
    void HideInGUI() noexcept;
    bool IsVisibleInGUI() const noexcept;
    std::type_index GetTypeIndex() const noexcept;
    bool IsChanged() const;

    virtual void SetValue(const cpgf::GVariant& value) = 0;
    virtual cpgf::GVariant GetValue() const = 0;

    size_t CountItem() const;
    IPrimitiveType* GetItemValue(size_t index) const;

protected:
    bool m_visibleInGUI = true;
    std::type_index m_typeIndex;
    std::vector<CompositeTypeItem> m_properties;
};

template<
    typename T,
    typename ItemType = std::remove_cvref_t<decltype(std::declval<T>()[0])>,
    typename Enable = std::enable_if_t<std::is_integral_v<ItemType> || std::is_floating_point_v<ItemType>>
    >
class CompositeTypeT final : public CompositeType {
public:
    using FieldType = ItemType;

public:
    CompositeTypeT(const std::vector<CompositeTypeItem>& properties)
        : CompositeType(std::type_index(typeid(T)), properties) {

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
};

}
