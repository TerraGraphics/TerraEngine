#include "middleware/gscheme/embedded/func_constant.h"

#include "rttr/reflection.h"
#include "middleware/gscheme/reflection/gs_metadata.h"
#include "middleware/gscheme/embedded/embedded_decl.h" // IWYU pragma: keep


namespace gs {

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

    registration::class_<FuncConstant>("Constant")(metadata(MetaTypes::CLASS, "Constant"))
        .constructor<>()
        .property_readonly("Result", &FuncConstant::Result)(metadata(MetaTypes::OUTPUT_PIN, "Result"))
        .property("Value", &FuncConstant::GetValue, &FuncConstant::SetValue)(metadata(MetaTypes::EMBEDDED_PROPERTY, "Value"))
        ;
}

}