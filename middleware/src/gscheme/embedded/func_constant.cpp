#include "middleware/gscheme/embedded/func_constant.h"

#include "middleware/gscheme/rttr/registration.h"
#include "middleware/gscheme/embedded/embedded_decl.h" // IWYU pragma: keep


float FuncConstant::GetValue() const noexcept {
    return m_value;
}

void FuncConstant::SetValue(float v) noexcept {
    m_value = v;
}

REFLECTION_IMPL(FuncConstant) {
    using namespace rttr;

    registration::class_<FuncConstant>("Constant")
        .constructor<>()
        .property("Value", &FuncConstant::GetValue, &FuncConstant::SetValue)
        ;
}
