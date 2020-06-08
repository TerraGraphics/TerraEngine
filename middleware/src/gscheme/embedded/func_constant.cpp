#include "middleware/gscheme/embedded/func_constant.h"

#include "rttr/reflection.h"
#include "middleware/gscheme/reflection/gs_metadata.h"
#include "middleware/gscheme/embedded/embedded_decl.h" // IWYU pragma: keep


namespace gs {

void FuncConstant::SetValue(const float v) noexcept {
    m_value = v;
}

void FuncConstant2::SetValue(const Eigen::Vector2f v) noexcept {
    m_value = v;
}

void FuncConstant3::SetValue(const Eigen::Vector3f v) noexcept {
    m_value = v;
}

void FuncConstant4::SetValue(const Eigen::Vector4f v) noexcept {
    m_value = v;
}

REFLECTION_IMPL(FuncConstant) {
    using namespace rttr;

    registration::class_<FuncConstant>("Constant")(metadata(MetaTypes::CLASS, "Constant"))
        .constructor<>()
        .property_readonly("Result", &FuncConstant::Result)(metadata(MetaTypes::OUTPUT_PIN, "Result"))
        .property("Value", &FuncConstant::GetValue, &FuncConstant::SetValue)(metadata(MetaTypes::EMBEDDED_PROPERTY, "Value"))
        ;

    registration::class_<FuncConstant2>("Constant2")(metadata(MetaTypes::CLASS, "Constant2"))
        .constructor<>()
        .property_readonly("Result", &FuncConstant2::Result)(metadata(MetaTypes::OUTPUT_PIN, "Result"))
        .property("Value", &FuncConstant2::GetValue, &FuncConstant2::SetValue)(metadata(MetaTypes::EMBEDDED_PROPERTY, "Value"))
        ;

    registration::class_<FuncConstant3>("Constant3")(metadata(MetaTypes::CLASS, "Constant3"))
        .constructor<>()
        .property_readonly("Result", &FuncConstant3::Result)(metadata(MetaTypes::OUTPUT_PIN, "Result"))
        .property("Value", &FuncConstant3::GetValue, &FuncConstant3::SetValue)(metadata(MetaTypes::EMBEDDED_PROPERTY, "Value"))
        ;

    registration::class_<FuncConstant4>("Constant4")(metadata(MetaTypes::CLASS, "Constant4"))
        .constructor<>()
        .property_readonly("Result", &FuncConstant4::Result)(metadata(MetaTypes::OUTPUT_PIN, "Result"))
        .property("Value", &FuncConstant4::GetValue, &FuncConstant4::SetValue)(metadata(MetaTypes::EMBEDDED_PROPERTY, "Value"))
        ;
}

}
