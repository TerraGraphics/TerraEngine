#pragma once

#include <typeindex>

#include "core/common/ctor.h"
#include "core/common/pimpl.h"


namespace gs {

class MetaType;
class MetaEnum;
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
    MetaType* GetType(std::type_index id);
    void AddType(std::type_index id, MetaType* metaType);

    MetaEnum* GetEnum(std::type_index id);
    void AddEnum(std::type_index id, MetaEnum* metaEnum);

private:
    struct Impl;
    Pimpl<Impl, 160, 8> impl;
};

}
