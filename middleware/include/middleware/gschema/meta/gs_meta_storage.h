#pragma once

#include <vector>
#include <typeindex>

#include "core/common/ctor.h"
#include "core/common/pimpl.h"


namespace gs {

class MetaType;
class MetaEnum;
class MetaClass;
class MetaStorage : Fixed {
private:
    MetaStorage();
    ~MetaStorage();

public:
    static MetaStorage& GetInstance() {
        static MetaStorage instance;

        return instance;
    }

public:
    using TDefineCallback = void(*)();

    void AddDefineTypesCallback(TDefineCallback func);
    void AddDefineClassesCallback(TDefineCallback func);
    void RunDefineCallbacks();

public:
    MetaClass* GetBaseClass(std::type_index id) const;
    void AddBaseClass(std::type_index id, MetaClass* metaClass);

    std::vector<const MetaClass*> GetClasses() const;
    MetaClass* GetClass(std::type_index id) const;
    void AddClass(std::type_index id, MetaClass* metaClass);

    MetaType* GetType(std::type_index id) const;
    void AddType(std::type_index id, MetaType* metaType);

    MetaEnum* GetEnum(std::type_index id) const;
    void AddEnum(std::type_index id, MetaEnum* metaEnum);

private:
    struct Impl;
    Pimpl<Impl, 272, 8> impl;
};

}
