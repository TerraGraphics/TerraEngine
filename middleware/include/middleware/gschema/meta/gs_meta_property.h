#pragma once

#include <string>
#include <typeinfo>
#include <string_view>

#include "core/common/ctor.h"
#include "middleware/gschema/meta/gs_meta_consts.h"


namespace cpgf {
class GVariant;
class GMetaProperty;
}

namespace gs {

class TypeInstanceEdit;
class MetaProperty : Fixed {
public:
    MetaProperty() = delete;
    MetaProperty(cpgf::GMetaProperty* property, std::string_view name, std::string_view displayName, PinTypes pinType, TypeInstanceEdit* typeInstance);
    ~MetaProperty();

    std::string_view GetName() const { return m_name; }
    std::string_view GetDisplayName() const { return m_displayName; }
    PinTypes GetPinType() const { return m_pinType; }
    TypeInstanceEdit* GetTypeInstance() const { return m_typeInstance; }
    const std::type_info& GetTypeInfo() const;

    cpgf::GVariant Get(const void* instance) const;
	void Set(void* instance, const cpgf::GVariant& value) const;

private:
    cpgf::GMetaProperty* m_property = nullptr;
    TypeInstanceEdit* m_typeInstance = nullptr;
    std::string m_name;
    std::string m_displayName;
    PinTypes m_pinType = PinTypes::EMBEDDED;
};

}
