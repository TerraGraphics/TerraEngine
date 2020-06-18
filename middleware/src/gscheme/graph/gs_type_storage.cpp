#include "middleware/gscheme/graph/gs_type_storage.h"

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
#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/graph/gs_convert.h"
#include "middleware/gscheme/graph/gs_metadata.h"
#include "middleware/gscheme/embedded/embedded.h" // IWYU pragma: keep
#include "middleware/gscheme/graph/gs_type_class.h"


#define CONVERT_FUNC(R, L) AddConvertFunc<BOOST_PP_TUPLE_ELEM(2, 1, L), BOOST_PP_TUPLE_ELEM(2, 0, L)>();

namespace gs {

struct TypeStorage::Impl {
    Impl();
    ~Impl();

    template <typename To, typename From> void AddConvertFunc();

    uint16_t m_countTypeClasses = 0;
    TypeClass* m_typeClasses;
    std::unordered_map<std::string_view, uint16_t> m_typeClassesIndex;
    std::unordered_map<uint16_t, std::function<cpgf::GVariant (const cpgf::GVariant&)>> m_convertFuncs;
};

TypeStorage::Impl::Impl() {
    const cpgf::GMetaClass* gMetaClass = cpgf::getGlobalMetaClass();
    for(size_t i=0; i!=gMetaClass->getClassCount(); ++i) {
        const cpgf::GMetaClass* metaClass = gMetaClass->getClassAt(i);
        if ((metaClass != nullptr) && (metaClass->getAnnotation(gs::MetaNames::CLASS) != nullptr)) {
            ++m_countTypeClasses;
        }
    }

    m_typeClasses = new TypeClass[m_countTypeClasses];
    uint16_t index = 0;
    for(size_t i=0; i!=gMetaClass->getClassCount(); ++i) {
        const cpgf::GMetaClass* metaClass = gMetaClass->getClassAt(i);
        if ((metaClass != nullptr) && (metaClass->getAnnotation(gs::MetaNames::CLASS) != nullptr)) {
            m_typeClasses[index].Create(metaClass);
            m_typeClassesIndex[m_typeClasses[index].GetName()] = index;
            ++index;
        }
    }

    BOOST_PP_LIST_FOR_EACH_PRODUCT(CONVERT_FUNC, 2, (UNIVERSAL_TYPES, UNIVERSAL_TYPES));
}

template <typename To, typename From> void TypeStorage::Impl::AddConvertFunc() {
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
    return GetTypeClass(GetTypeClassIndex(name));
}

uint16_t TypeStorage::GetTypeClassIndex(std::string_view name) {
    if (auto it = impl->m_typeClassesIndex.find(name); it != impl->m_typeClassesIndex.cend()) {
        return it->second;
    }

    throw EngineError("gs::TypeStorage::GetTypeClassIndex: wrong name = {}, not found node with this name", name);
}

const TypeClass* TypeStorage::TypeClassesBegin() const noexcept {
    return impl->m_typeClasses;
}

const TypeClass* TypeStorage::TypeClassesEnd() const noexcept {
    return impl->m_typeClasses + impl->m_countTypeClasses;
}

}
