#pragma once

#include "core/common/ctor.h"


namespace gs {

class FuncSum : Fixed {
public:
    FuncSum() = default;
    ~FuncSum() = default;

    float Result() const noexcept;

    float GetA() const noexcept { return m_a; }
    void SetA(const float v) noexcept;
    float GetB() const noexcept { return m_b; }
    void SetB(const float v) noexcept;

private:
    float m_a = 0.f;
    float m_b = 0.f;
};

}
