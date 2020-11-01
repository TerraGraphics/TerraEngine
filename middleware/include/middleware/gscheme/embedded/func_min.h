#pragma once

#include "core/common/ctor.h"
#include "middleware/gscheme/graph/gs_types.h"


namespace gs {

class FuncMin : Fixed {
public:
    FuncMin() = default;
    ~FuncMin() = default;

    UniversalType Result() const;

    UniversalType GetA() const { return m_a; }
    void SetA(const UniversalType v) { m_a = v; }
    UniversalType GetB() const { return m_b; }
    void SetB(const UniversalType v) { m_b = v; }

private:
    UniversalType m_a = 0.f;
    UniversalType m_b = 0.f;
};

}
