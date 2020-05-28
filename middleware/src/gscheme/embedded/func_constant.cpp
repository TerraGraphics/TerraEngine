#include "middleware/gscheme/embedded/func_constant.h"

#include "middleware/gscheme/rttr/registration.h"
#include "middleware/gscheme/reflection/gs_metadata.h"
#include "middleware/gscheme/embedded/embedded_decl.h" // IWYU pragma: keep


float FuncConstant::Result() const noexcept {
    return m_value;
}

float FuncConstant::GetValue() const noexcept {
    return m_value;
}

void FuncConstant::SetValue(float v) noexcept {
    m_value = v;
}

REFLECTION_IMPL(FuncConstant) {
    using namespace rttr;

    registration::class_<FuncConstant>("Constant")(metadata(GSMetaTypes::GS_CLASS, true))
        .constructor<>()
        .method("Result", &FuncConstant::Result)
        .property("Value", &FuncConstant::GetValue, &FuncConstant::SetValue)(metadata(GSMetaTypes::GS_EMBEDDED_PROPERTY, true))
        ;
}
