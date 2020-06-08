#include "middleware/gscheme/embedded/func_sum.h"

#include "rttr/reflection.h"
#include "middleware/gscheme/reflection/gs_metadata.h"
#include "middleware/gscheme/embedded/embedded_decl.h" // IWYU pragma: keep


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

REFLECTION_IMPL(FuncSum) {
    using namespace rttr;

    registration::class_<FuncSum>("Sum")(metadata(MetaTypes::CLASS, "Sum"))
        .constructor<>()
        .property_readonly("Result", &FuncSum::Result)(metadata(MetaTypes::OUTPUT_PIN, "Result"))
        .property("A", &FuncSum::GetA, &FuncSum::SetA)(metadata(MetaTypes::INPUT_PIN, "A"))
        .property("B", &FuncSum::GetB, &FuncSum::SetB)(metadata(MetaTypes::INPUT_PIN, "B"))
        ;
}

}
