#pragma once

#include <vector>
#include <string>
#include <string_view>

#include "core/common/ctor.h"


namespace cpgf {
class GMetaProperty;
}

namespace gs {

class MetaClass : Fixed {
public:
    using TCtor = void* (*)();

public:
    MetaClass() = default;
    MetaClass(TCtor& ctor, std::string_view name, std::string_view displayName, MetaClass* baseClass = nullptr);
    ~MetaClass() = default;

    bool IsBaseClass() const { return m_ctor == nullptr; }

    std::string_view GetName() const { return m_name; }
    std::string_view GetDisplayName() const { return m_displayName; }
    void* Create() const;

    void AddProperty(const cpgf::GMetaProperty* property);
    std::vector<const cpgf::GMetaProperty*> GetProperties() const;

private:
    MetaClass* m_baseClass = nullptr;
    TCtor m_ctor = nullptr;
    std::string m_name;
    std::string m_displayName;
    std::vector<const cpgf::GMetaProperty*> m_properties;
};

}
