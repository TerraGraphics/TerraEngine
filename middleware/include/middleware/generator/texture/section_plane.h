#pragma once

#include "core/common/ctor.h"
#include "core/math/generator_type.h"


class SectionPlaneX0Y : Fixed {
public:
    SectionPlaneX0Y() = default;
    ~SectionPlaneX0Y() = default;

    math::Generator2d Result() const;

    float GetOffset() const { return m_offset; }
    void SetOffset(const float v) { m_offset = v; }

    math::Generator3d GetInput() const { return m_input; }
    void SetInput(const math::Generator3d& v) { m_input = v; }

private:
    float m_offset = 0;
    math::Generator3d m_input;
};
