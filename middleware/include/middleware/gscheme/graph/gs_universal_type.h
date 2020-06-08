#pragma once

#include <variant>

#include "rttr/rttr.h"
#include "eigen/core.h"
#include "core/math/generator_type.h"


namespace gs {

using UniversalType = std::variant<float, Eigen::Vector2f, Eigen::Vector3f, Eigen::Vector4f, math::Generator2D, math::Generator3D>;

}
