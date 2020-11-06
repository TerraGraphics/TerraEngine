#include "middleware/gscheme/meta/gs_meta_storage.h"

#include <utility>
#include <unordered_map>

#include "core/common/hash.h"
#include "core/common/exception.h"
#include "middleware/gscheme/meta/gs_meta_type.h"


namespace gs {

struct MetaStorage::Impl : Fixed {
    Impl() = default;
    ~Impl();

    std::unordered_map<std::type_index, MetaType*> m_types;
};

MetaStorage::Impl::~Impl() {
    for (auto& [_, v]: m_types) {
        delete v;
    }
}

MetaStorage::MetaStorage()
    : impl() {
}

MetaStorage::~MetaStorage() {

}

MetaType* MetaStorage::GetType(std::type_index id) {
    if (auto it = impl->m_types.find(id); it != impl->m_types.cend()) {
        return it->second;
    }

    throw EngineError("gs::MetaStorage::GetType: metaType with name = {} not found", id.name());
}

void MetaStorage::AddType(std::type_index id, MetaType* metaType) {
    if (impl->m_types.find(id) != impl->m_types.cend()) {
        throw EngineError("gs::MetaStorage::AddType: metaType with name = {} already exists", id.name());
    }

    impl->m_types[id] = metaType;
}

}
