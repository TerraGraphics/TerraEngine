#pragma once

#include "core/common/ctor.h"
#include "middleware/gscheme/graph/gs_types.h"


namespace gs {

class FuncAdd : Fixed {
public:
    FuncAdd() = default;
    ~FuncAdd() = default;

    UniversalType Result() const;

    UniversalType GetA() const { return m_a; }
    void SetA(const UniversalType v);
    UniversalType GetB() const { return m_b; }
    void SetB(const UniversalType v);

private:
    UniversalType m_a = 0.f;
    UniversalType m_b = 0.f;
};

}
