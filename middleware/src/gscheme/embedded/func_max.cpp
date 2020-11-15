#include "middleware/gscheme/embedded/func_max.h"

#include <variant>
#include <algorithm>
#include <type_traits>

#include "eigen/core.h"
#include "core/common/exception.h"
#include "core/math/generator_type.h"
#include "core/math/generator_type_operators.h"
#include "middleware/gscheme/graph/gs_types_fmt.h" // IWYU pragma: keep
#include "middleware/gscheme/graph/gs_types_convert.h"


namespace std {
    template<typename T>
    static T maxImpl(const T& a, const T& b) {
        T r;
        const auto* aData = a.data();
        const auto* bData = b.data();
        auto* rData = r.data();
        for (int i=0; i!=a.size(); ++i) {
            rData[i] = std::max(aData[i], bData[i]);
        }

        return r;
    }

    static Eigen::Vector2f max(const Eigen::Vector2f& a, const Eigen::Vector2f& b) {
        return maxImpl<Eigen::Vector2f>(a, b);
    }

    static Eigen::Vector3f max(const Eigen::Vector3f& a, const Eigen::Vector3f& b) {
        return maxImpl<Eigen::Vector3f>(a, b);
    }

    static Eigen::Vector4f max(const Eigen::Vector4f& a, const Eigen::Vector4f& b) {
        return maxImpl<Eigen::Vector4f>(a, b);
    }
}

namespace gs {

namespace {

template<typename T>
    static math::Generator2D MaxGeneratorT(const math::Generator2D& generator, const T& value) {
        if constexpr (IsFloat<T>) {
            return std::max(generator, value);
        } else if constexpr (IsVector<T>) {
            return std::max(generator, value[0]);
        }
    }

template<typename T>
    static math::Generator3D MaxGeneratorT(const math::Generator3D& generator, const T& value) {
        if constexpr (IsFloat<T>) {
            return std::max(generator, value);
        } else if constexpr (IsVector<T>) {
            return std::max(generator, value[0]);
        }
    }

template<typename TMin, typename TMax>
    static TMax MaxOfMinMaxT(const TMin& valMin, const TMax& valMax) {
        if constexpr (!CanConvert<TMax, TMin>) {
            throw EngineError("types {} and {} are not compatible", GetTypeId<TMin>(), GetTypeId<TMax>());
        } else if constexpr (std::is_same<TMin, TMax>::value) {
            return std::max(valMax, valMin);
        } else if constexpr (IsGenerator2d<TMax> || IsGenerator3d<TMax>) {
            return MaxGeneratorT(valMax, valMin);
        } else {
            return std::max(valMax, ConvertTo<TMax, TMin>(valMin));
        }
    }
}

UniversalType FuncMax::Result() const {
    return std::visit([](auto&& a, auto&& b) -> UniversalType {
        if constexpr (GetTypeId<std::remove_cvref_t<decltype(a)>>() <= GetTypeId<std::remove_cvref_t<decltype(b)>>()) {
            return UniversalType(MaxOfMinMaxT(a, b));
        } else {
            return UniversalType(MaxOfMinMaxT(b, a));
        }
    }, m_a, m_b);
}

}
