#pragma once

#include "dg/math.h"


// convert m to dg::float3x3, inverse, transpose and convert to dg::float4x4
dg::float4x4 MakeNormalMatrix4x4(const dg::float4x4& m);

// convert m to dg::float3x3, inverse, transpose
dg::float3x3 MakeNormalMatrix3x3(const dg::float4x4& m);
