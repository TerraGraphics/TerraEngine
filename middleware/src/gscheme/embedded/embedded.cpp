#include "middleware/gscheme/embedded/embedded.h"

#include "cpgf/metadefine.h"
#include "middleware/gscheme/graph/gs_metadata.h"
#include "middleware/gscheme/embedded/func_sum.h"
#include "middleware/gscheme/embedded/func_add.h"
#include "middleware/gscheme/embedded/func_constant.h"
#include "middleware/gscheme/graph/gs_convert_storage.h"



REFLECTION_IMPL(gs::EmbeddedFuncs) {
    using namespace gs;
    using namespace cpgf;

    GDefineMetaClass<FuncConstant>
        ::define("Constant")
            ._annotation(MetaNames::CLASS)._element(MetaNames::PRETTY_NAME, "Constant")
        ._constructor<void * ()>()
        ._property("Result", &FuncConstant::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::PRETTY_NAME, "Result")
        ._property("Value", &FuncConstant::GetValue, &FuncConstant::SetValue)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::EMBEDDED)._element(MetaNames::PRETTY_NAME, "Value")
    ;

    GDefineMetaClass<FuncConstant2>
        ::define("Constant2")
            ._annotation(MetaNames::CLASS)._element(MetaNames::PRETTY_NAME, "Constant2")
        ._constructor<void * ()>()
        ._property("Result", &FuncConstant2::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::PRETTY_NAME, "Result")
        ._property("Value", &FuncConstant2::GetValue, &FuncConstant2::SetValue)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::EMBEDDED)._element(MetaNames::PRETTY_NAME, "Value")
    ;

    GDefineMetaClass<FuncConstant3>
        ::define("Constant3")
            ._annotation(MetaNames::CLASS)._element(MetaNames::PRETTY_NAME, "Constant3")
        ._constructor<void * ()>()
        ._property("Result", &FuncConstant3::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::PRETTY_NAME, "Result")
        ._property("Value", &FuncConstant3::GetValue, &FuncConstant3::SetValue)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::EMBEDDED)._element(MetaNames::PRETTY_NAME, "Value")
    ;

    GDefineMetaClass<FuncConstant4>
        ::define("Constant4")
            ._annotation(MetaNames::CLASS)._element(MetaNames::PRETTY_NAME, "Constant4")
        ._constructor<void * ()>()
        ._property("Result", &FuncConstant4::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::PRETTY_NAME, "Result")
        ._property("Value", &FuncConstant4::GetValue, &FuncConstant4::SetValue)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::EMBEDDED)._element(MetaNames::PRETTY_NAME, "Value")
    ;

    GDefineMetaClass<FuncAdd>
        ::define("Add")
            ._annotation(MetaNames::CLASS)._element(MetaNames::PRETTY_NAME, "Add")
        ._constructor<void * ()>()
        ._property("Result", &FuncAdd::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::PRETTY_NAME, "Result")
        ._property("A", &FuncAdd::GetA, &FuncAdd::SetA)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::INPUT)._element(MetaNames::PRETTY_NAME, "A")
        ._property("B", &FuncAdd::GetB, &FuncAdd::SetB)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::INPUT)._element(MetaNames::PRETTY_NAME, "B")
    ;

    GDefineMetaClass<TypeAdd>
        ::Policy<MakePolicy<GMetaRuleDefaultConstructorAbsent>>
        ::define("TypeAdd")
            ._annotation(MetaNames::TYPE_CLASS)._element(MetaNames::IMPL_CLASS, "Add")
        ._constructor<void * (ConvertStorage*)>()
        ._property("Result", &TypeAdd::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)
        ._property("A", &TypeAdd::GetA, &TypeAdd::SetA)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::INPUT)
        ._property("B", &TypeAdd::GetB, &TypeAdd::SetB)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::INPUT)
        ._method("IsValid", &TypeAdd::IsValid)
        ._method("Apply", &TypeAdd::Apply)
        ._method("Reset", &TypeAdd::Reset)
    ;

    GDefineMetaClass<FuncSum>
        ::define("Sum")
            ._annotation(MetaNames::CLASS)._element(MetaNames::PRETTY_NAME, "Sum")
        ._constructor<void * ()>()
        ._property("Result", &FuncSum::Result, 0)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::OUTPUT)._element(MetaNames::PRETTY_NAME, "Result")
        ._property("A", &FuncSum::GetA, &FuncSum::SetA)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::INPUT)._element(MetaNames::PRETTY_NAME, "A")
        ._property("B", &FuncSum::GetB, &FuncSum::SetB)
            ._annotation(MetaNames::PIN)._element(MetaNames::PIN_TYPE, PinTypes::INPUT)._element(MetaNames::PRETTY_NAME, "B")
    ;
}
