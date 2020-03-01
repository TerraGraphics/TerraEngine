#pragma once

#include "core/math/basic.h"

namespace math {

    bool QuadEquation(dg::float3 p, dg::float2& result);
    dg::float4 GetPlaneByPointAndNormal(dg::float3 point, dg::float3 n);
    dg::float4 GetPlaneByPointAndTwoVec(dg::float3 point, dg::float3 v0, dg::float3 v1);
    bool IntersectionRayAndCylinder(dg::float3 rayStart, dg::float3 rayDir, Axis axisUp, float radius, float height);
    bool IntersectionRayAndPlane(dg::float3 rayStart, dg::float3 rayDir, dg::float4 plane, dg::float3& result);
}