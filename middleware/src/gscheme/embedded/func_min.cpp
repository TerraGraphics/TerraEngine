#include "middleware/gscheme/embedded/func_min.h"

#include <variant>
#include <type_traits>

#include "eigen/core.h"
#include "core/common/exception.h"
#include "core/math/generator_type.h"
#include "middleware/gscheme/graph/gs_types_fmt.h"
#include "middleware/gscheme/graph/gs_types_convert.h"
#include "middleware/gscheme/graph/gs_types_convert_storage.h"


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
    static math::Generator2D TMinGenerator2d(const math::Generator2D& generator, const T& value) {
        if constexpr (IsFloat<T>) {
            return math::Generator2D([generator, v = static_cast<double>(value)](double x, double y) -> double {
                return std::min(generator(x, y), v);
            });
        } else if constexpr (IsVector<T>) {
            return math::Generator2D([generator, v = static_cast<double>(value[0])](double x, double y) -> double {
                return std::min(generator(x, y), v);
            });
        } else if constexpr (IsGenerator2d<T>) {
            return math::Generator2D([generator, value](double x, double y) -> double {
                return std::min(generator(x, y), value(x, y));
            });
        }
    }

template<typename T>
    static math::Generator3D TMinGenerator3d(const math::Generator3D& generator, const T& value) {
        if constexpr (IsFloat<T>) {
            return math::Generator3D([generator, v = static_cast<double>(value)](double x, double y, double z) -> double {
                return std::min(generator(x, y, z), v);
            });
        } else if constexpr (IsVector<T>) {
            return math::Generator3D([generator, v = static_cast<double>(value[0])](double x, double y, double z) -> double {
                return std::min(generator(x, y, z), v);
            });
        } else if constexpr (IsGenerator3d<T>) {
            return math::Generator3D([generator, value](double x, double y, double z) -> double {
                return std::min(generator(x, y, z), value(x, y, z));
            });
        }
    }

template<typename TMin, typename TMax>
    static TMax TMinOfMinMax(const TMin& valMin, const TMax& valMax) {
        if constexpr (!CanConvert<TMax, TMin>) {
            throw EngineError("types {} and {} are not compatible", GetTypeId<TMin>(), GetTypeId<TMax>());
        } else if constexpr (IsGenerator2d<TMax>) {
            return TMinGenerator2d(valMax, valMin);
        } else if constexpr (IsGenerator3d<TMax>) {
            return TMinGenerator3d(valMax, valMin);
        } else if constexpr (std::is_same<TMin, TMax>::value) {
            return std::min(valMin, valMax);
        } else {
            return std::min(ConvertTo<TMax, TMin>(valMin), TMax(valMax));
        }
    }
}

UniversalType FuncMin::Result() const {
    return std::visit([](auto&& a, auto&& b) -> UniversalType {
        if constexpr (GetTypeId<std::remove_cvref_t<decltype(a)>>() <= GetTypeId<std::remove_cvref_t<decltype(b)>>()) {
            return UniversalType(TMinOfMinMax(a, b));
        } else {
            return UniversalType(TMinOfMinMax(b, a));
        }
    }, m_a, m_b);
}

}
