#pragma once

#include "core/math/types.h"

namespace math {

    bool IntersectionRayAndCylinder(dg::float3 rayStart, dg::float3 rayDir, Axis axisUp, float radius, float height);
    bool IntersectionRayAndPlane(dg::float3 rayStart, dg::float3 rayDir, PlaneT<float> plane, dg::float3& result);
}
