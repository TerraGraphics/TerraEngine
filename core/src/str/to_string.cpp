#include "core/str/to_string.h"

#include <string>

#include "core/str/format.h"


std::string ToString(const dg::float4x4& x) {
    std::string formatStr("mat4x4(\n  ({: .2f}, {: .2f}, {: .2f}, {: .2f}),\n  ({: .2f}, {: .2f}, {: .2f}, {: .2f}),\n  ({: .2f}, {: .2f}, {: .2f}, {: .2f}),\n  ({: .2f}, {: .2f}, {: .2f}, {: .2f})\n)");

    return fmt::format(formatStr,
        x.m00, x.m01, x.m02, x.m03,
        x.m10, x.m11, x.m12, x.m13,
        x.m20, x.m21, x.m22, x.m23,
        x.m30, x.m31, x.m32, x.m33);
}
