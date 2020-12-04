#pragma once

#include <vector>
#include <string>
#include <typeindex>
#include <string_view>

#include "core/common/ctor.h"
#include "middleware/gschema/meta/gs_meta_consts.h"


namespace gs {

class MetaProperty;
class TypeInstanceEdit;
class MetaPropertyDataBase;
class MetaClass : Fixed {
public:
    using TCtor = void* (*)();
    using TDtor = void (*)(void*);
    using TIsPinEnableInGUI = bool (*)(void* instance, std::string_view name);

public:
    MetaClass() = default;
    MetaClass(TCtor ctor, TDtor dtor, std::string_view name, std::string_view displayName, MetaClass* baseClass = nullptr);
    ~MetaClass();

    bool IsBaseClass() const { return m_ctor == nullptr; }

    std::string_view GetName() const { return m_name; }
    std::string_view GetDisplayName() const { return m_displayName; }

    void* CreateInstance() const;
    void DestroyInstance(void* instance) const;

    void SetIsPinEnableInGUI(TIsPinEnableInGUI func);
    bool IsPinEnableInGUI(void* instance, std::string_view name) const;

    void AddProperty(MetaPropertyDataBase* data, std::type_index typeIndex, std::string_view name, std::string_view displayName, PinTypes pinType, TypeInstanceEdit* typeInstance);
    std::vector<const MetaProperty*> GetProperties() const;

private:
    MetaClass* m_baseClass = nullptr;
    TCtor m_ctor = nullptr;
    TDtor m_dtor = nullptr;
    TIsPinEnableInGUI m_isPinEnableInGUI = nullptr;
    std::string m_name;
    std::string m_displayName;
    std::vector<const MetaProperty*> m_properties;
};

}
