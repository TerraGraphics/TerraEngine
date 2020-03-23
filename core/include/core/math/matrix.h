#pragma once

#include <cstdint>
#include "core/dg/math.h"

namespace math {

dg::float3 GetTransform(const dg::float4x4& m);
void SetTransform(dg::float4x4& m, const dg::float3& value);

}