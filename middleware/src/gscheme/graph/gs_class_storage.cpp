#include "middleware/gscheme/graph/gs_class_storage.h"

#include <utility>
#include <cstddef>
#include <unordered_map>

#include "cpgf/metaclass.h"
#include "core/common/hash.h"
#include "core/common/exception.h"
#include "middleware/generator/gs_register.h" // IWYU pragma: keep
#include "middleware/gscheme/graph/gs_class.h"
#include "middleware/gscheme/graph/gs_metadata.h"
#include "middleware/gscheme/embedded/embedded.h" // IWYU pragma: keep
#include "middleware/gscheme/graph/gs_types_convert_storage.h"


namespace gs {

struct ClassStorage::Impl {
    Impl();
    ~Impl();

    uint16_t m_countClasses = 0;
    Class* m_classes;
    TypesConvertStorage m_typesConvertStorage;
    std::unordered_map<std::string_view, uint16_t> m_classesIndex;
};

ClassStorage::Impl::Impl() {
    const cpgf::GMetaClass* gMetaClass = cpgf::getGlobalMetaClass();

    for(size_t i=0; i!=gMetaClass->getClassCount(); ++i) {
        const cpgf::GMetaClass* metaClass = gMetaClass->getClassAt(i);
        if (metaClass == nullptr) {
            continue;
        }
        if (metaClass->getAnnotation(gs::MetaNames::CLASS) != nullptr) {
            ++m_countClasses;
        }
    }

    uint16_t index = 0;
    m_classes = new Class[m_countClasses];
    for(size_t i=0; i!=gMetaClass->getClassCount(); ++i) {
        const cpgf::GMetaClass* metaClass = gMetaClass->getClassAt(i);
        if ((metaClass != nullptr) && (metaClass->getAnnotation(gs::MetaNames::CLASS) != nullptr)) {
            m_classes[index].Create(metaClass, &m_typesConvertStorage);
            m_classesIndex[m_classes[index].GetName()] = index;
            ++index;
        }
    }
}

ClassStorage::Impl::~Impl() {
    m_classesIndex.clear();
    if (m_classes != nullptr) {
        delete[] m_classes;
    }
}

ClassStorage::ClassStorage() {

}

ClassStorage::~ClassStorage() {

}

Class* ClassStorage::GetClass(uint16_t classIndex) {
    if (classIndex >= impl->m_countClasses) {
        throw EngineError(
            "gs::ClassStorage::GetClass: wrong classIndex = {}, max value = {}", classIndex, impl->m_countClasses - 1);
    }

    return &impl->m_classes[classIndex];
}

Class* ClassStorage::GetClass(std::string_view name) {
    return GetClass(GetClassIndex(name));
}

uint16_t ClassStorage::GetClassIndex(std::string_view name) {
    if (auto it = impl->m_classesIndex.find(name); it != impl->m_classesIndex.cend()) {
        return it->second;
    }

    throw EngineError("gs::ClassStorage::GetClassIndex: wrong name = {}, not found node with this name", name);
}

const Class* ClassStorage::ClassesBegin() const noexcept {
    return impl->m_classes;
}

const Class* ClassStorage::ClassesEnd() const noexcept {
    return impl->m_classes + impl->m_countClasses;
}

}
