#include "middleware/gscheme/graph/gs_class_storage.h"

#include <utility>
#include <variant>
#include <cstddef>
#include <functional>
#include <unordered_map>
#include <boost/preprocessor/list/for_each_product.hpp>

#include "eigen/core.h"
#include "cpgf/metaclass.h"
#include "core/common/hash.h"
#include "core/common/exception.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_class.h"
#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/graph/gs_convert.h"
#include "middleware/gscheme/graph/gs_metadata.h"
#include "middleware/gscheme/embedded/embedded.h" // IWYU pragma: keep


#define CONVERT_FUNC(R, L) AddConvertFunc<BOOST_PP_TUPLE_ELEM(2, 1, L), BOOST_PP_TUPLE_ELEM(2, 0, L)>();

namespace gs {

struct ClassStorage::Impl {
    Impl();
    ~Impl();

    template <typename To, typename From> void AddConvertFunc();

    uint16_t m_countClasses = 0;
    Class* m_classes;
    std::unordered_map<std::string_view, uint16_t> m_classesIndex;
    std::unordered_map<uint16_t, std::function<cpgf::GVariant (const cpgf::GVariant&)>> m_convertFuncs;
};

ClassStorage::Impl::Impl() {
    const cpgf::GMetaClass* gMetaClass = cpgf::getGlobalMetaClass();
    for(size_t i=0; i!=gMetaClass->getClassCount(); ++i) {
        const cpgf::GMetaClass* metaClass = gMetaClass->getClassAt(i);
        if ((metaClass != nullptr) && (metaClass->getAnnotation(gs::MetaNames::CLASS) != nullptr)) {
            ++m_countClasses;
        }
    }

    m_classes = new Class[m_countClasses];
    uint16_t index = 0;
    for(size_t i=0; i!=gMetaClass->getClassCount(); ++i) {
        const cpgf::GMetaClass* metaClass = gMetaClass->getClassAt(i);
        if ((metaClass != nullptr) && (metaClass->getAnnotation(gs::MetaNames::CLASS) != nullptr)) {
            m_classes[index].Create(metaClass);
            m_classesIndex[m_classes[index].GetName()] = index;
            ++index;
        }
    }

    BOOST_PP_LIST_FOR_EACH_PRODUCT(CONVERT_FUNC, 2, (UNIVERSAL_TYPES, UNIVERSAL_TYPES));
}

template <typename To, typename From> void ClassStorage::Impl::AddConvertFunc() {
    if constexpr (gs::CanConvert<To, From>) {
        constexpr uint16_t fromId = (static_cast<uint16_t>(GetTypeId<From>()) << uint16_t(8));
        constexpr uint16_t fromIdU = (static_cast<uint16_t>(ToUniversalTypeId(GetTypeId<From>())) << uint16_t(8));
        constexpr uint16_t toId = static_cast<uint16_t>(GetTypeId<To>());
        constexpr uint16_t toIdU = static_cast<uint16_t>(ToUniversalTypeId(GetTypeId<To>()));

        m_convertFuncs[(fromId | toId)] = [](const cpgf::GVariant& value) -> cpgf::GVariant {
            return cpgf::GVariant(ConvertTo<To, From>(cpgf::fromVariant<From>(value)));
        };

        m_convertFuncs[(fromId | toIdU)] = [](const cpgf::GVariant& value) -> cpgf::GVariant {
            return cpgf::GVariant(UniversalType(ConvertTo<To, From>(cpgf::fromVariant<From>(value))));
        };

        m_convertFuncs[(fromIdU | toId)] = [](const cpgf::GVariant& value) -> cpgf::GVariant {
            return cpgf::GVariant(ConvertTo<To, From>(std::get<From>(cpgf::fromVariant<UniversalType>(value))));
        };

        m_convertFuncs[(fromIdU | toIdU)] = [](const cpgf::GVariant& value) -> cpgf::GVariant {
            return cpgf::GVariant(UniversalType(ConvertTo<To, From>(std::get<From>(cpgf::fromVariant<UniversalType>(value)))));
        };
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
