#include "middleware/gscheme/embedded/func_sum.h"

#include "middleware/gscheme/rttr/registration.h"
#include "middleware/gscheme/embedded/embedded_decl.h" // IWYU pragma: keep


float FuncSum::Result() const noexcept {
    return m_a + m_b;
}

float FuncSum::GetA() const noexcept {
    return m_a;
}

void FuncSum::SetA(float v) noexcept {
    m_a = v;
}

float FuncSum::GetB() const noexcept {
    return m_b;
}

void FuncSum::SetB(float v) noexcept {
    m_b = v;
}

REFLECTION_IMPL(FuncSum) {
    using namespace rttr;

    registration::class_<FuncSum>("Sum")
        .constructor<>()
        .method("Result", &FuncSum::Result)
        .property("A", &FuncSum::GetA, &FuncSum::SetA)
        .property("B", &FuncSum::GetB, &FuncSum::SetB)
        ;
}
