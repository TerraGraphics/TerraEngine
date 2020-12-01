#include "middleware/gschema/meta/gs_meta_storage.h"

#include <utility>
#include <unordered_map>

#include "core/common/hash.h"
#include "core/common/exception.h"
#include "middleware/gschema/meta/gs_meta_type.h"
#include "middleware/gschema/meta/gs_meta_class.h"


namespace gs {

struct MetaStorage::Impl : Fixed {
    Impl() = default;
    ~Impl();

    std::vector<TDefineCallback> m_defineTypesFuncs;
    std::vector<TDefineCallback> m_defineClassesFuncs;
    std::unordered_map<std::type_index, MetaType*> m_types;
    std::unordered_map<std::type_index, MetaEnum*> m_enums;
    std::unordered_map<std::type_index, MetaClass*> m_classes;
    std::unordered_map<std::type_index, MetaClass*> m_baseClasses;
};

MetaStorage::Impl::~Impl() {
    for (auto& [_, v]: m_types) {
        delete v;
    }
    for (auto& [_, v]: m_enums) {
        delete v;
    }
    for (auto& [_, v]: m_classes) {
        delete v;
    }
    for (auto& [_, v]: m_baseClasses) {
        delete v;
    }
}

MetaStorage::MetaStorage() {
}

MetaStorage::~MetaStorage() {
}

void MetaStorage::AddDefineTypesCallback(TDefineCallback func) {
    impl->m_defineTypesFuncs.push_back(func);
}

void MetaStorage::AddDefineClassesCallback(TDefineCallback func) {
    impl->m_defineClassesFuncs.push_back(func);
}

void MetaStorage::RunDefineCallbacks() {
    for (auto& func: impl->m_defineTypesFuncs) {
        func();
    }
    for (auto& func: impl->m_defineClassesFuncs) {
        func();
    }
    impl->m_defineTypesFuncs.clear();
    impl->m_defineClassesFuncs.clear();
}

MetaClass* MetaStorage::GetBaseClass(std::type_index id) const {
    if (auto it = impl->m_baseClasses.find(id); it != impl->m_baseClasses.cend()) {
        return it->second;
    }

    throw EngineError("gs::MetaStorage::GetBaseClass: MetaClass with name = {} not found", id.name());
}

void MetaStorage::AddBaseClass(std::type_index id, MetaClass* metaClass) {
    if (!metaClass->IsBaseClass()) {
        throw EngineError("gs::MetaStorage::AddBaseClass: MetaClass with name = {} is not base class", id.name());
    }

    if (impl->m_baseClasses.find(id) != impl->m_baseClasses.cend()) {
        throw EngineError("gs::MetaStorage::AddBaseClass: MetaClass with name = {} already exists by id", id.name());
    }

    impl->m_baseClasses[id] = metaClass;
}

std::vector<const MetaClass*> MetaStorage::GetClasses() const {
    std::vector<const MetaClass*> result;
    result.reserve(impl->m_classes.size());
    for (auto& [_, v]: impl->m_classes) {
        result.push_back(v);
    }

    return result;
}

MetaClass* MetaStorage::GetClass(std::type_index id) const {
    if (auto it = impl->m_classes.find(id); it != impl->m_classes.cend()) {
        return it->second;
    }

    throw EngineError("gs::MetaStorage::GetClass: MetaClass with name = {} not found", id.name());
}

void MetaStorage::AddClass(std::type_index id, MetaClass* metaClass) {
    if (metaClass->IsBaseClass()) {
        throw EngineError("gs::MetaStorage::AddBaseClass: MetaClass with name = {} is base class", id.name());
    }

    auto name = metaClass->GetName();
    for (auto& [index, v]: impl->m_classes) {
        if (index == id) {
            throw EngineError("gs::MetaStorage::AddClass: MetaClass with name = {} already exists by id", name);
        }
        if (v->GetName() == name) {
            throw EngineError("gs::MetaStorage::AddClass: MetaClass with name = {} already exists by name", name);
        }
    }

    impl->m_classes[id] = metaClass;
}

MetaType* MetaStorage::GetType(std::type_index id) const {
    if (auto it = impl->m_types.find(id); it != impl->m_types.cend()) {
        return it->second;
    }

    throw EngineError("gs::MetaStorage::GetType: MetaType with name = {} not found", id.name());
}

void MetaStorage::AddType(std::type_index id, MetaType* metaType) {
    if (impl->m_types.find(id) != impl->m_types.cend()) {
        throw EngineError("gs::MetaStorage::AddType: MetaType with name = {} already exists by id", id.name());
    }

    impl->m_types[id] = metaType;
}

MetaEnum* MetaStorage::GetEnum(std::type_index id) const {
    if (auto it = impl->m_enums.find(id); it != impl->m_enums.cend()) {
        return it->second;
    }

    throw EngineError("gs::MetaStorage::GetEnum: MetaEnum with name = {} not found", id.name());
}

void MetaStorage::AddEnum(std::type_index id, MetaEnum* metaEnum) {
    if (impl->m_enums.find(id) != impl->m_enums.cend()) {
        throw EngineError("gs::MetaStorage::AddEnum: MetaEnum with name = {} already exists by id", id.name());
    }

    impl->m_enums[id] = metaEnum;
}

}
