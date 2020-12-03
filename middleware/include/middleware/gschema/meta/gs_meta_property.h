#pragma once

#include <string>
#include <typeindex>
#include <string_view>

#include "core/common/ctor.h"
#include "middleware/gschema/meta/gs_meta_consts.h"


namespace cpgf {
class GVariant;
}

namespace gs {

class TypeInstanceEdit;
class MetaPropertyDataBase;
class MetaProperty : Fixed {
public:
    MetaProperty() = delete;
    MetaProperty(MetaPropertyDataBase* data, std::type_index typeIndex, std::string_view name, std::string_view displayName, PinTypes pinType, TypeInstanceEdit* typeInstance);
    ~MetaProperty();

    std::string_view GetName() const { return m_name; }
    std::string_view GetDisplayName() const { return m_displayName; }
    PinTypes GetPinType() const { return m_pinType; }
    TypeInstanceEdit* GetTypeInstance() const { return m_typeInstance; }
    std::type_index GetTypeIndex() const { return m_typeIndex; }

    cpgf::GVariant Get(const void* instance) const;
	void Set(void* instance, const cpgf::GVariant& value) const;

private:
    MetaPropertyDataBase* m_data = nullptr;
    TypeInstanceEdit* m_typeInstance = nullptr;
    std::type_index m_typeIndex;
    std::string m_name;
    std::string m_displayName;
    PinTypes m_pinType = PinTypes::EMBEDDED;
};

}
