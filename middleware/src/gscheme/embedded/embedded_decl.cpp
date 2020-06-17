#include "middleware/gscheme/embedded/embedded_decl.h"

#include "rttr/reflection.h"
#include "middleware/gscheme/graph/gs_metadata.h"
#include "middleware/gscheme/embedded/func_sum.h"
#include "middleware/gscheme/embedded/func_add.h"
#include "middleware/gscheme/embedded/func_constant.h"


REFLECTION_IMPL(gs::Embedded) {
    using namespace gs;
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

    registration::class_<FuncAdd>("Add")(metadata(MetaTypes::CLASS, "Add"))
        .constructor<>()
        .property_readonly("Result", &FuncAdd::Result)(metadata(MetaTypes::OUTPUT_PIN, "Result"))
        .property("A", &FuncAdd::GetA, &FuncAdd::SetA)(metadata(MetaTypes::INPUT_PIN, "A"))
        .property("B", &FuncAdd::GetB, &FuncAdd::SetB)(metadata(MetaTypes::INPUT_PIN, "B"))
        ;

    registration::class_<FuncSum>("Sum")(metadata(MetaTypes::CLASS, "Sum"))
        .constructor<>()
        .property_readonly("Result", &FuncSum::Result)(metadata(MetaTypes::OUTPUT_PIN, "Result"))
        .property("A", &FuncSum::GetA, &FuncSum::SetA)(metadata(MetaTypes::INPUT_PIN, "A"))
        .property("B", &FuncSum::GetB, &FuncSum::SetB)(metadata(MetaTypes::INPUT_PIN, "B"))
        ;
}
