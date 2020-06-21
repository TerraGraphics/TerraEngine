#include "middleware/gscheme/embedded/func_add.h"

#include <variant>
#include <type_traits>

#include "eigen/core.h"
#include "core/common/exception.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_convert.h"
#include "middleware/gscheme/graph/gs_convert_storage.h"


namespace gs {

namespace {

template<typename T>
    static math::Generator2D TSumGenerator2D(const math::Generator2D& generator, const T& value) {
        if constexpr (IsFloat<T>) {
            return math::Generator2D([generator, value](double x, double y) -> double {
                return (generator(x, y) + value);
            });
        } else if constexpr (IsVector<T>) {
            return math::Generator2D([generator, v = value[0]](double x, double y) -> double {
                return (generator(x, y) + v);
            });
        } else if constexpr (IsGenerator2D<T>) {
            return math::Generator2D([generator, value](double x, double y) -> double {
                return (generator(x, y) + value(x, y));
            });
        }
    }

template<typename T>
    static math::Generator3D TSumGenerator3D(const math::Generator3D& generator, const T& value) {
        if constexpr (IsFloat<T>) {
            return math::Generator3D([generator, value](double x, double y, double z) -> double {
                return (generator(x, y, z) + value);
            });
        } else if constexpr (IsVector<T>) {
            return math::Generator3D([generator, v = value[0]](double x, double y, double z) -> double {
                return (generator(x, y, z) + v);
            });
        } else if constexpr (IsGenerator3D<T>) {
            return math::Generator3D([generator, value](double x, double y, double z) -> double {
                return (generator(x, y, z) + value(x, y, z));
            });
        }
    }

template<typename TMin, typename TMax>
    static TMax TSumMinMax(const TMin& valMin, const TMax& valMax) {
        if constexpr (!CanConvert<TMax, TMin>) {
            throw EngineError("gs::FuncAdd::Result: types are not compatible");
        } else if constexpr (IsGenerator2D<TMax>) {
            return TSumGenerator2D(valMax, valMin);
        } else if constexpr (IsGenerator3D<TMax>) {
            return TSumGenerator3D(valMax, valMin);
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

TypeAdd::TypeAdd(ConvertStorage* convertStorage)
    : m_convertStorage(convertStorage) {

}

TypeId TypeAdd::Result() const {
    return MaxTypeId(m_a, m_b);
}

bool TypeAdd::IsValid() {
    auto maxId = MaxTypeId(m_a, m_b);
    return m_convertStorage->CanConvert((maxId != m_a) ? m_a : m_b, maxId);
}

void TypeAdd::Apply() {
    m_originA = m_a;
    m_originB = m_b;
}

void TypeAdd::Reset() {
    m_a = m_originA;
    m_b = m_originB;
}

}
