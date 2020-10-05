#include "middleware/gscheme/graph/gs_types_convert_storage.h"

#include <cstdint>
#include <variant>
#include <utility>
#include <unordered_map>
#include <boost/preprocessor/list/for_each.hpp>
#include <boost/preprocessor/list/for_each_product.hpp>

#include "eigen/core.h"
#include "cpgf/metaclass.h"
#include "core/common/exception.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/graph/gs_types_convert.h"
#include "middleware/gscheme/graph/gs_types_convert_func.h"


#define CONVERT_FUNC(R, L) AddConvertFunc<BOOST_PP_TUPLE_ELEM(2, 1, L), BOOST_PP_TUPLE_ELEM(2, 0, L)>();
#define CONVERT_FUNC_TO_UNIVERSAL(r, data, elem) AddConvertFuncToUniversal<elem>();

namespace {
    inline constexpr uint16_t KeyId(gs::TypeId from, gs::TypeId to) {
        return static_cast<uint16_t>((static_cast<uint16_t>(from) << uint16_t(8)) | static_cast<uint16_t>(to));
    }
}

namespace gs {

struct TypesConvertStorage::Impl {
    Impl();

    template <typename To, typename From> void AddConvertFunc();
    template <typename From> void AddConvertFuncToUniversal();

    std::unordered_map<uint16_t, ConvertFunc> m_convertFuncs;
};

TypesConvertStorage::Impl::Impl() {
    BOOST_PP_LIST_FOR_EACH_PRODUCT(CONVERT_FUNC, 2, (UNIVERSAL_TYPES, UNIVERSAL_TYPES));
    BOOST_PP_LIST_FOR_EACH(CONVERT_FUNC_TO_UNIVERSAL, _, UNIVERSAL_TYPES);

    m_convertFuncs[KeyId(TypeId::UniversalType, TypeId::UniversalType)] = [](const cpgf::GVariant& value) -> cpgf::GVariant {
        return cpgf::createVariant<UniversalType>(cpgf::fromVariant<UniversalType>(value), true);
    };
}

template <typename To, typename From> void TypesConvertStorage::Impl::AddConvertFunc() {
    if constexpr (gs::CanConvert<To, From>) {
        m_convertFuncs[KeyId(GetTypeId<From>(), GetTypeId<To>())] = [](const cpgf::GVariant& value) -> cpgf::GVariant {
            return cpgf::createVariant<To>(ConvertTo<To, From>(cpgf::fromVariant<From>(value)), true);
        };

        m_convertFuncs[KeyId(ToUniversalTypeId(GetTypeId<From>()), GetTypeId<To>())] = [](const cpgf::GVariant& value) -> cpgf::GVariant {
            return cpgf::createVariant<To>(ConvertTo<To, From>(std::get<From>(cpgf::fromVariant<UniversalType>(value))), true);
        };
    }
}

template <typename From> void TypesConvertStorage::Impl::AddConvertFuncToUniversal() {
    m_convertFuncs[KeyId(GetTypeId<From>(), TypeId::UniversalType)] = [](const cpgf::GVariant& value) -> cpgf::GVariant {
        return cpgf::createVariant<UniversalType>(UniversalType(cpgf::fromVariant<From>(value)), true);
    };
}

TypesConvertStorage::TypesConvertStorage() {

}

TypesConvertStorage::~TypesConvertStorage() {

}

bool TypesConvertStorage::CanConvert(TypeId from, TypeId to) const {
    return (impl->m_convertFuncs.find(KeyId(from, to)) != impl->m_convertFuncs.cend());
}

ConvertFunc TypesConvertStorage::GetConvertFunc(TypeId from, TypeId to) const {
    if (auto it = impl->m_convertFuncs.find(KeyId(from, to)); it != impl->m_convertFuncs.cend()) {
        return it->second;
    }

    throw EngineError("TypesConvertStorage: function converting from type {} to type {} does not exist", from, to);
}

}
