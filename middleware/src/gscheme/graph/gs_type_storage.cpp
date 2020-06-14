#include "middleware/gscheme/graph/gs_type_storage.h"

#include <string>
#include <utility>
#include <variant>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <boost/preprocessor/list/for_each_product.hpp>

#include "rttr/rttr.h"
#include "eigen/core.h"
#include "core/common/hash.h"
#include "core/common/exception.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/graph/gs_convert.h"
#include "middleware/gscheme/graph/gs_type_class.h"
#include "middleware/gscheme/reflection/gs_metadata.h"


#define RTTR_CONVERT_FUNC(R, L) AddConvertFunc<BOOST_PP_TUPLE_ELEM(2, 1, L), BOOST_PP_TUPLE_ELEM(2, 0, L)>();

namespace gs {

struct TypeStorage::Impl {
    Impl();
    ~Impl();

    template <typename To, typename From> void AddConvertFunc();

    uint16_t m_countTypeClasses = 0;
    TypeClass* m_typeClasses;
    std::unordered_map<std::string_view, uint16_t> m_typeClassesIndex;
    std::unordered_map<uint16_t, std::function<rttr::variant (const rttr::variant&)>> m_convertFuncs;
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
            m_typeClassesIndex[m_typeClasses[index].GetName()] = index;

            ++index;
        }
    }

    BOOST_PP_LIST_FOR_EACH_PRODUCT(RTTR_CONVERT_FUNC, 2, (UNIVERSAL_TYPES, UNIVERSAL_TYPES));
}

template <typename To, typename From> void TypeStorage::Impl::AddConvertFunc() {
    if constexpr (gs::CanConvert<To, From>) {
        constexpr uint16_t fromId = (static_cast<uint16_t>(GetTypeId<From>()) << uint16_t(8));
        constexpr uint16_t fromIdU = (static_cast<uint16_t>(ToUniversalTypeId(GetTypeId<From>())) << uint16_t(8));
        constexpr uint16_t toId = static_cast<uint16_t>(GetTypeId<To>());
        constexpr uint16_t toIdU = static_cast<uint16_t>(ToUniversalTypeId(GetTypeId<To>()));

        m_convertFuncs[(fromId | toId)] = [](const rttr::variant& value) -> rttr::variant {
            return rttr::variant(ConvertTo<To, From>(value.get_wrapped_value<From>()));
        };

        m_convertFuncs[(fromId | toIdU)] = [](const rttr::variant& value) -> rttr::variant {
            return rttr::variant(UniversalType(ConvertTo<To, From>(value.get_wrapped_value<From>())));
        };

        m_convertFuncs[(fromIdU | toId)] = [](const rttr::variant& value) -> rttr::variant {
            return rttr::variant(ConvertTo<To, From>(std::get<From>(value.get_wrapped_value<UniversalType>())));
        };

        m_convertFuncs[(fromIdU | toIdU)] = [](const rttr::variant& value) -> rttr::variant {
            return rttr::variant(UniversalType(ConvertTo<To, From>(std::get<From>(value.get_wrapped_value<UniversalType>()))));
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
