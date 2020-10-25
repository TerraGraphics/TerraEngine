#pragma once

#include "core/common/ctor.h"
#include "core/math/generator_type.h"


class ChessCube : Fixed {
public:
    ChessCube() = default;
    ~ChessCube() = default;

    math::Generator3d Result() const;
};
