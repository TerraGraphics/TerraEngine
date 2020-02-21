#pragma once

#include "core/dg/dg.h"
#include "core/dg/math.h"

namespace math {

    bool QuadEquation(dg::float3 p, dg::float2& result);
    bool IntersectionRayAndCylinder0Z(dg::float3 rayStart, dg::float3 rayDir, float radius, float height);

}
