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
    static MetaStorage& getInstance() {
        static MetaStorage instance;

        return instance;
    }

public:
    MetaType* GetType(std::type_index id);
    void AddType(std::type_index id, MetaType* metaType);

    MetaEnum* GetEnum(std::type_index id);
    void AddEnum(std::type_index id, MetaEnum* metaEnum);

private:
    struct Impl;
    Pimpl<Impl, 112, 8> impl;
};

}
