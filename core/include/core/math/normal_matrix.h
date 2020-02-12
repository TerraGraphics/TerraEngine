#pragma once

#include "core/dg/math.h"


// convert m to dg::float3x3, inverse, transpose and convert to dg::float4x4
Diligent::float4x4 MakeNormalMatrix4x4(const Diligent::float4x4& m);

// convert m to dg::float3x3, inverse, transpose
Diligent::float3x3 MakeNormalMatrix3x3(const Diligent::float4x4& m);
