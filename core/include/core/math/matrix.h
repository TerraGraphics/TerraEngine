#pragma once

#include <cstdint>
#include "core/dg/math.h"

namespace math {

dg::float3 GetTranslation(const dg::float4x4& m);
void SetTranslation(dg::float4x4& m, const dg::float3& value);

dg::float3 GetScale(const dg::float4x4& m);
void SetScale(dg::float4x4& m, const dg::float3& value);

dg::float3 GetRotate(dg::float4x4 m);
void SetRotate(dg::float4x4& m, const dg::float3& value);

}
