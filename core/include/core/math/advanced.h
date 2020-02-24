#pragma once

#include "core/math/basic.h"

namespace math {

    bool QuadEquation(dg::float3 p, dg::float2& result);
    bool IntersectionRayAndCylinder(dg::float3 rayStart, dg::float3 rayDir, Axis axisUp, float radius, float height);

}
