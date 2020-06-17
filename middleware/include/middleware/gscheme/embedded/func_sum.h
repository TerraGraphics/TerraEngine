#pragma once

#include "core/common/ctor.h"


namespace gs {

class FuncSum : Fixed {
public:
    FuncSum() = default;
    ~FuncSum() = default;

    float Result() const;

    float GetA() const { return m_a; }
    void SetA(const float v);
    float GetB() const { return m_b; }
    void SetB(const float v);

private:
    float m_a = 0.f;
    float m_b = 0.f;
};

}
