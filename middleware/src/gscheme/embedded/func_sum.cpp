#include "middleware/gscheme/embedded/func_sum.h"


namespace gs {

float FuncSum::Result() const noexcept {
    return m_a + m_b;
}

float FuncSum::GetA() const noexcept {
    return m_a;
}

void FuncSum::SetA(const float v) noexcept {
    m_a = v;
}

float FuncSum::GetB() const noexcept {
    return m_b;
}

void FuncSum::SetB(const float v) noexcept {
    m_b = v;
}

}
