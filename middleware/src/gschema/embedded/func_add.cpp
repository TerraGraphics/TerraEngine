#include "middleware/gschema/embedded/func_add.h"

#include <variant>
#include <type_traits>

#include "eigen/core.h"
#include "core/common/exception.h"
#include "core/math/generator_type.h"
#include "core/math/generator_type_operators.h"
#include "middleware/gschema/graph/gs_types_fmt.h" // IWYU pragma: keep
#include "middleware/gschema/graph/gs_types_convert.h"


namespace gs {

namespace {

template<typename T>
    static math::Generator2D SumGeneratorT(const math::Generator2D& generator, const T& value) {
        if constexpr (IsFloat<T>) {
            return generator + value;
        } else if constexpr (IsVector<T>) {
            return generator + value[0];
        }
    }

template<typename T>
    static math::Generator3D SumGeneratorT(const math::Generator3D& generator, const T& value) {
        if constexpr (IsFloat<T>) {
            return generator + value;
        } else if constexpr (IsVector<T>) {
            return generator + value[0];
        }
    }

template<typename TMin, typename TMax>
    static TMax SumOfMinMaxT(const TMin& valMin, const TMax& valMax) {
        if constexpr (!CanConvert<TMax, TMin>) {
            throw EngineError("types {} and {} are not compatible", GetTypeId<TMin>(), GetTypeId<TMax>());
        } else if constexpr (std::is_same_v<TMin, TMax>) {
            return valMax + valMin;
        } else if constexpr (IsGenerator2d<TMax> || IsGenerator3d<TMax>) {
            return SumGeneratorT(valMax, valMin);
        } else {
            return valMax + ConvertTo<TMax, TMin>(valMin);
        }
    }
}

UniversalType FuncAdd::Result() const {
    return std::visit([](auto&& a, auto&& b) -> UniversalType {
        if constexpr (GetTypeId<std::remove_cvref_t<decltype(a)>>() <= GetTypeId<std::remove_cvref_t<decltype(b)>>()) {
            return UniversalType(SumOfMinMaxT(a, b));
        } else {
            return UniversalType(SumOfMinMaxT(b, a));
        }
    }, m_a, m_b);
}

}
