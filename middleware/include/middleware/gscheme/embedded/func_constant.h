#pragma once

#include "core/common/ctor.h"


class FuncConstant : Fixed {
public:
    FuncConstant() = default;
    ~FuncConstant() = default;

    float GetValue() const noexcept;
    void SetValue(float v) noexcept;

private:
    float m_value = 0.f;
};
