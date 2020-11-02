#include "middleware/gscheme/embedded/embedded.h"

#include "middleware/gscheme/graph/gs_define.h"
#include "middleware/gscheme/graph/gs_metadata.h"
#include "middleware/gscheme/embedded/func_add.h"
#include "middleware/gscheme/embedded/func_min.h"
#include "middleware/gscheme/embedded/func_max.h"
#include "middleware/gscheme/embedded/func_base.h"
#include "middleware/gscheme/embedded/func_constant.h"


REFLECTION_IMPL(gs::EmbeddedFuncs) {
    using namespace gs;
    using namespace cpgf;

    GDefineMetaClass<FuncConstant>
        ::define("Constant")
            ._annotation(MetaNames::CLASS)._element(MetaNames::DISPLAY_NAME, "Constant")
        ._constructor<void * ()>()
        ._property("Result", &FuncConstant::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::DISPLAY_NAME, "Result")
        ._property("Value", &FuncConstant::GetValue, &FuncConstant::SetValue)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::EMBEDDED)._element(MetaNames::DISPLAY_NAME, "Value")
    ;

    GDefineMetaClass<FuncConstant2>
        ::define("Constant2")
            ._annotation(MetaNames::CLASS)._element(MetaNames::DISPLAY_NAME, "Constant2")
        ._constructor<void * ()>()
        ._property("Result", &FuncConstant2::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::DISPLAY_NAME, "Result")
        ._property("Value", &FuncConstant2::GetValue, &FuncConstant2::SetValue)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::EMBEDDED)._element(MetaNames::DISPLAY_NAME, "Value")
    ;

    GDefineMetaClass<FuncConstant3>
        ::define("Constant3")
            ._annotation(MetaNames::CLASS)._element(MetaNames::DISPLAY_NAME, "Constant3")
        ._constructor<void * ()>()
        ._property("Result", &FuncConstant3::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::DISPLAY_NAME, "Result")
        ._property("Value", &FuncConstant3::GetValue, &FuncConstant3::SetValue)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::EMBEDDED)._element(MetaNames::DISPLAY_NAME, "Value")
    ;

    GDefineMetaClass<FuncConstant4>
        ::define("Constant4")
            ._annotation(MetaNames::CLASS)._element(MetaNames::DISPLAY_NAME, "Constant4")
        ._constructor<void * ()>()
        ._property("Result", &FuncConstant4::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::DISPLAY_NAME, "Result")
        ._property("Value", &FuncConstant4::GetValue, &FuncConstant4::SetValue)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::EMBEDDED)._element(MetaNames::DISPLAY_NAME, "Value")
    ;

    GDefineMetaClass<FuncBinaryUniversal>
        ::define("Meta")
        ._property("A", &FuncBinaryUniversal::GetA, &FuncBinaryUniversal::SetA)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::INPUT)._element(MetaNames::DISPLAY_NAME, "A")
        ._property("B", &FuncBinaryUniversal::GetB, &FuncBinaryUniversal::SetB)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::INPUT)._element(MetaNames::DISPLAY_NAME, "B")
    ;

    GDefineMetaClass<FuncAdd, FuncBinaryUniversal>
        ::define("Add")
            ._annotation(MetaNames::CLASS)._element(MetaNames::DISPLAY_NAME, "Add")
        ._constructor<void * ()>()
        ._property("Result", &FuncAdd::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::DISPLAY_NAME, "Result")
    ;

    GDefineMetaClass<FuncMin, FuncBinaryUniversal>
        ::define("Min")
            ._annotation(MetaNames::CLASS)._element(MetaNames::DISPLAY_NAME, "Min")
        ._constructor<void * ()>()
        ._property("Result", &FuncMin::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::DISPLAY_NAME, "Result")
    ;

    GDefineMetaClass<FuncMax, FuncBinaryUniversal>
        ::define("Max")
            ._annotation(MetaNames::CLASS)._element(MetaNames::DISPLAY_NAME, "Max")
        ._constructor<void * ()>()
        ._property("Result", &FuncMax::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::DISPLAY_NAME, "Result")
    ;
}
