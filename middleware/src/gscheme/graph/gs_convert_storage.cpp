#include "middleware/gscheme/graph/gs_convert_storage.h"

#include <cstdint>
#include <variant>
#include <functional>
#include <unordered_map>
#include <boost/preprocessor/list/for_each.hpp>
#include <boost/preprocessor/list/for_each_product.hpp>

#include "eigen/core.h"
#include "cpgf/metaclass.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/graph/gs_types_convert.h"


#define CONVERT_FUNC(R, L) AddConvertFunc<BOOST_PP_TUPLE_ELEM(2, 1, L), BOOST_PP_TUPLE_ELEM(2, 0, L)>();
#define CONVERT_FUNC_TO_UNIVERSAL(r, data, elem) AddConvertFuncToUniversal<elem>();

namespace {
    inline constexpr uint16_t KeyId(gs::TypeId from, gs::TypeId to) {
        return static_cast<uint16_t>((static_cast<uint16_t>(from) << uint16_t(8)) | static_cast<uint16_t>(to));
    }
}

namespace gs {

struct ConvertStorage::Impl {
    Impl();

    template <typename To, typename From> void AddConvertFunc();
    template <typename From> void AddConvertFuncToUniversal();

    std::unordered_map<uint16_t, std::function<cpgf::GVariant (const cpgf::GVariant&)>> m_convertFuncs;
};

ConvertStorage::Impl::Impl() {
    BOOST_PP_LIST_FOR_EACH_PRODUCT(CONVERT_FUNC, 2, (UNIVERSAL_TYPES, UNIVERSAL_TYPES));
    BOOST_PP_LIST_FOR_EACH(CONVERT_FUNC_TO_UNIVERSAL, _, UNIVERSAL_TYPES);

    m_convertFuncs[KeyId(TypeId::UniversalType, TypeId::UniversalType)] = [](const cpgf::GVariant& value) -> cpgf::GVariant {
        return cpgf::GVariant(cpgf::fromVariant<UniversalType>(value));
    };
}

template <typename To, typename From> void ConvertStorage::Impl::AddConvertFunc() {
    if constexpr (gs::CanConvert<To, From>) {
        m_convertFuncs[KeyId(GetTypeId<From>(), GetTypeId<To>())] = [](const cpgf::GVariant& value) -> cpgf::GVariant {
            return cpgf::GVariant(ConvertTo<To, From>(cpgf::fromVariant<From>(value)));
        };

        m_convertFuncs[KeyId(ToUniversalTypeId(GetTypeId<From>()), GetTypeId<To>())] = [](const cpgf::GVariant& value) -> cpgf::GVariant {
            return cpgf::GVariant(ConvertTo<To, From>(std::get<From>(cpgf::fromVariant<UniversalType>(value))));
        };
    }
}

template <typename From> void ConvertStorage::Impl::AddConvertFuncToUniversal() {
    m_convertFuncs[KeyId(GetTypeId<From>(), TypeId::UniversalType)] = [](const cpgf::GVariant& value) -> cpgf::GVariant {
        return cpgf::GVariant(UniversalType(cpgf::fromVariant<From>(value)));
    };
}

ConvertStorage::ConvertStorage() {

}

ConvertStorage::~ConvertStorage() {

}

bool ConvertStorage::CanConvert(TypeId from, TypeId to) const {
    return (impl->m_convertFuncs.find(KeyId(from, to)) != impl->m_convertFuncs.cend());
}

}
