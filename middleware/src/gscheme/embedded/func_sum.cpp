#include "middleware/gscheme/embedded/func_sum.h"


namespace gs {

float FuncSum::Result() const {
    return m_a + m_b;
}

void FuncSum::SetA(const float v) {
    m_a = v;
}

void FuncSum::SetB(const float v) {
    m_b = v;
}

}
