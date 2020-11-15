#include "middleware/gscheme/embedded/func_min.h"

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
    static T minImpl(const T& a, const T& b) {
        T r;
        const auto* aData = a.data();
        const auto* bData = b.data();
        auto* rData = r.data();
        for (int i=0; i!=a.size(); ++i) {
            rData[i] = std::min(aData[i], bData[i]);
        }

        return r;
    }

    static Eigen::Vector2f min(const Eigen::Vector2f& a, const Eigen::Vector2f& b) {
        return minImpl<Eigen::Vector2f>(a, b);
    }

    static Eigen::Vector3f min(const Eigen::Vector3f& a, const Eigen::Vector3f& b) {
        return minImpl<Eigen::Vector3f>(a, b);
    }

    static Eigen::Vector4f min(const Eigen::Vector4f& a, const Eigen::Vector4f& b) {
        return minImpl<Eigen::Vector4f>(a, b);
    }
}

namespace gs {

namespace {

template<typename T>
    static math::Generator2D MinGeneratorT(const math::Generator2D& generator, const T& value) {
        if constexpr (IsFloat<T>) {
            return std::min(generator, value);
        } else if constexpr (IsVector<T>) {
            return std::min(generator, value[0]);
        }
    }

template<typename T>
    static math::Generator3D MinGeneratorT(const math::Generator3D& generator, const T& value) {
        if constexpr (IsFloat<T>) {
            return std::min(generator, value);
        } else if constexpr (IsVector<T>) {
            return std::min(generator, value[0]);
        }
    }

template<typename TMin, typename TMax>
    static TMax MinOfMinMaxT(const TMin& valMin, const TMax& valMax) {
        if constexpr (!CanConvert<TMax, TMin>) {
            throw EngineError("types {} and {} are not compatible", GetTypeId<TMin>(), GetTypeId<TMax>());
        } else if constexpr (std::is_same<TMin, TMax>::value) {
            return std::min(valMax, valMin);
        } else if constexpr (IsGenerator2d<TMax> || IsGenerator3d<TMax>) {
            return MinGeneratorT(valMax, valMin);
        } else {
            return std::min(valMax, ConvertTo<TMax, TMin>(valMin));
        }
    }
}

UniversalType FuncMin::Result() const {
    return std::visit([](auto&& a, auto&& b) -> UniversalType {
        if constexpr (GetTypeId<std::remove_cvref_t<decltype(a)>>() <= GetTypeId<std::remove_cvref_t<decltype(b)>>()) {
            return UniversalType(MinOfMinMaxT(a, b));
        } else {
            return UniversalType(MinOfMinMaxT(b, a));
        }
    }, m_a, m_b);
}

}
