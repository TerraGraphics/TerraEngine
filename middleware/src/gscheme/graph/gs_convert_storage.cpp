#include "middleware/gscheme/graph/gs_convert_storage.h"

#include <variant>
#include <functional>
#include <unordered_map>
#include <boost/preprocessor/list/for_each_product.hpp>

#include "eigen/core.h"
#include "cpgf/metaclass.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/graph/gs_convert.h"


#define CONVERT_FUNC(R, L) AddConvertFunc<BOOST_PP_TUPLE_ELEM(2, 1, L), BOOST_PP_TUPLE_ELEM(2, 0, L)>();

namespace {
    inline constexpr uint16_t KeyId(gs::TypeId from, gs::TypeId to) {
        return static_cast<uint16_t>((static_cast<uint16_t>(from) << uint16_t(8)) | static_cast<uint16_t>(to));
    }
}

namespace gs {

struct ConvertStorage::Impl {
    Impl();

    template <typename To, typename From> void AddConvertFunc();

    std::unordered_map<uint16_t, std::function<cpgf::GVariant (const cpgf::GVariant&)>> m_convertFuncs;
};

ConvertStorage::Impl::Impl() {
    BOOST_PP_LIST_FOR_EACH_PRODUCT(CONVERT_FUNC, 2, (UNIVERSAL_TYPES, UNIVERSAL_TYPES));
}

template <typename To, typename From> void ConvertStorage::Impl::AddConvertFunc() {
    if constexpr (gs::CanConvert<To, From>) {
        constexpr auto fromId = GetTypeId<From>();
        constexpr auto fromIdU = ToUniversalTypeId(GetTypeId<From>());
        constexpr auto toId = GetTypeId<To>();
        constexpr auto toIdU = ToUniversalTypeId(GetTypeId<To>());

        m_convertFuncs[KeyId(fromId, toId)] = [](const cpgf::GVariant& value) -> cpgf::GVariant {
            return cpgf::GVariant(ConvertTo<To, From>(cpgf::fromVariant<From>(value)));
        };

        m_convertFuncs[KeyId(fromId, toIdU)] = [](const cpgf::GVariant& value) -> cpgf::GVariant {
            return cpgf::GVariant(UniversalType(ConvertTo<To, From>(cpgf::fromVariant<From>(value))));
        };

        m_convertFuncs[KeyId(fromIdU, toId)] = [](const cpgf::GVariant& value) -> cpgf::GVariant {
            return cpgf::GVariant(ConvertTo<To, From>(std::get<From>(cpgf::fromVariant<UniversalType>(value))));
        };

        m_convertFuncs[KeyId(fromIdU, toIdU)] = [](const cpgf::GVariant& value) -> cpgf::GVariant {
            return cpgf::GVariant(UniversalType(ConvertTo<To, From>(std::get<From>(cpgf::fromVariant<UniversalType>(value)))));
        };
    }
}

ConvertStorage::ConvertStorage() {

}

ConvertStorage::~ConvertStorage() {

}

bool ConvertStorage::CanConvert(TypeId from, TypeId to) {
    return (impl->m_convertFuncs.find(KeyId(from, to)) != impl->m_convertFuncs.cend());
}

}
