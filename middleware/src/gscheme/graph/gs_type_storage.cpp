#include "middleware/gscheme/graph/gs_type_storage.h"

#include <string>
#include <utility>
#include <unordered_map>
#include <unordered_set>

#include "rttr/rttr.h"
#include "core/common/hash.h"
#include "core/common/exception.h"
#include "middleware/gscheme/graph/gs_type_class.h"
#include "middleware/gscheme/reflection/gs_metadata.h"


namespace gs {

struct TypeStorage::Impl {
    Impl();
    ~Impl();

    uint16_t m_countTypeClasses = 0;
    TypeClass* m_typeClasses;
    std::unordered_map<std::string_view, TypeClass*> m_typeClassesIndex;
};

TypeStorage::Impl::Impl() {
    std::unordered_set<std::string> names;
    for(const auto& t : rttr::type::get_types()) {
        if (t.is_valid() && t.get_metadata(MetaTypes::CLASS).is_valid()) {
            if (!names.insert(t.get_name().to_string()).second) {
                throw EngineError("gs::TypeStorage::ctor: type classes have a duplicate name = '{}'", t.get_name().to_string());
            }
            ++m_countTypeClasses;
        }
    }

    m_typeClasses = new TypeClass[m_countTypeClasses];
    uint16_t index = 0;
    for(const auto& t : rttr::type::get_types()) {
        if (t.is_valid() && t.get_metadata(MetaTypes::CLASS).is_valid()) {
            m_typeClasses[index].Create(t);
            m_typeClassesIndex[m_typeClasses[index].GetName()] = &m_typeClasses[index];

            ++index;
        }
    }
}

TypeStorage::Impl::~Impl() {
    m_typeClassesIndex.clear();
    if (m_typeClasses != nullptr) {
        delete[] m_typeClasses;
    }
}

TypeStorage::TypeStorage() {

}

TypeStorage::~TypeStorage() {

}

TypeClass* TypeStorage::GetTypeClass(uint16_t typeClassIndex) {
    if (typeClassIndex >= impl->m_countTypeClasses) {
        throw EngineError(
            "gs::TypeStorage::GetTypeClass: wrong typeClassIndex = {}, max value = {}", typeClassIndex, impl->m_countTypeClasses - 1);
    }

    return &impl->m_typeClasses[typeClassIndex];
}

TypeClass* TypeStorage::GetTypeClass(std::string_view name) {
    if (auto it = impl->m_typeClassesIndex.find(name); it != impl->m_typeClassesIndex.cend()) {
        return it->second;
    }

    throw EngineError("gs::TypeStorage::GetTypeClass: wrong name = {}, not found node with this name", name);
}

const TypeClass* TypeStorage::TypeClassesBegin() const noexcept {
    return impl->m_typeClasses;
}

const TypeClass* TypeStorage::TypeClassesEnd() const noexcept {
    return impl->m_typeClasses + impl->m_countTypeClasses;
}

}
