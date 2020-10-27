#include "middleware/gscheme/embedded/func_add.h"

#include <variant>
#include <type_traits>

#include "eigen/core.h"
#include "core/common/exception.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_types_convert.h"
#include "middleware/gscheme/graph/gs_types_convert_storage.h"


namespace gs {

namespace {

template<typename T>
    static math::Generator2D TSumGenerator2d(const math::Generator2D& generator, const T& value) {
        if constexpr (IsFloat<T>) {
            return math::Generator2D([generator, value](double x, double y) -> double {
                return (generator(x, y) + value);
            });
        } else if constexpr (IsVector<T>) {
            return math::Generator2D([generator, v = value[0]](double x, double y) -> double {
                return (generator(x, y) + v);
            });
        } else if constexpr (IsGenerator2d<T>) {
            return math::Generator2D([generator, value](double x, double y) -> double {
                return (generator(x, y) + value(x, y));
            });
        }
    }

template<typename T>
    static math::Generator3D TSumGenerator3d(const math::Generator3D& generator, const T& value) {
        if constexpr (IsFloat<T>) {
            return math::Generator3D([generator, value](double x, double y, double z) -> double {
                return (generator(x, y, z) + value);
            });
        } else if constexpr (IsVector<T>) {
            return math::Generator3D([generator, v = value[0]](double x, double y, double z) -> double {
                return (generator(x, y, z) + v);
            });
        } else if constexpr (IsGenerator3d<T>) {
            return math::Generator3D([generator, value](double x, double y, double z) -> double {
                return (generator(x, y, z) + value(x, y, z));
            });
        }
    }

template<typename TMin, typename TMax>
    static TMax TSumMinMax(const TMin& valMin, const TMax& valMax) {
        if constexpr (!CanConvert<TMax, TMin>) {
            throw EngineError("gs::FuncAdd::Result: types are not compatible");
        } else if constexpr (IsGenerator2d<TMax>) {
            return TSumGenerator2d(valMax, valMin);
        } else if constexpr (IsGenerator3d<TMax>) {
            return TSumGenerator3d(valMax, valMin);
        } else if constexpr (std::is_same_v<TMin, TMax>) {
            return TMax(valMin + valMax);
        } else {
            return TMax(ConvertTo<TMax, TMin>(valMin) + valMax);
        }
    }

}

UniversalType FuncAdd::Result() const {
    return std::visit([](auto&& a, auto&& b) -> UniversalType {
        using typeA = std::remove_cvref_t<decltype(a)>;
        using typeB = std::remove_cvref_t<decltype(b)>;

        if constexpr (GetTypeId<typeA>() <= GetTypeId<typeB>()) {
            return UniversalType(TSumMinMax(a, b));
        } else {
            return UniversalType(TSumMinMax(b, a));
        }
    }, m_a, m_b);
}

TypeAdd::TypeAdd(const TypesConvertStorage* typesConvertStorage)
    : m_typesConvertStorage(typesConvertStorage) {

}

TypeId TypeAdd::Result() const {
    return MaxTypeId(m_a, m_b);
}

bool TypeAdd::IsValid() {
    auto maxId = MaxTypeId(m_a, m_b);
    return m_typesConvertStorage->CanConvert((maxId != m_a) ? m_a : m_b, maxId);
}

}
