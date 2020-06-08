#pragma once

#include "core/common/ctor.h"


namespace gs {

class FuncSum : Fixed {
public:
    FuncSum() = default;
    ~FuncSum() = default;

    float Result() const noexcept;

    float GetA() const noexcept;
    void SetA(const float v) noexcept;
    float GetB() const noexcept;
    void SetB(const float v) noexcept;

private:
    float m_a = 0.f;
    float m_b = 0.f;
};

}
