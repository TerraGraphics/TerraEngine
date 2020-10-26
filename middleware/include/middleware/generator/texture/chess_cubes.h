#pragma once

#include "core/common/ctor.h"
#include "core/math/generator_type.h"


class ChessCubes : Fixed {
public:
    ChessCubes() = default;
    ~ChessCubes() = default;

    math::Generator3d Result() const;

    float GetSize() const { return m_size; }
    void SetSize(const float v);

private:
    float m_size = 10.f;
};
