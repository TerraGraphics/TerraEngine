#pragma once

#include "core/math/types.h"

namespace math {

    bool IsIntersection(const RayT<double>& ray, const CylinderT<double>& cylinder);
    bool IsIntersection(const RayT<float>& ray, const CylinderT<float>& cylinder);

    bool IsIntersection(const RayT<double>& ray, const PlaneT<double>& plane, dg::double3& result);
    bool IsIntersection(const RayT<float>& ray, const PlaneT<float>& plane, dg::float3& result);

}
