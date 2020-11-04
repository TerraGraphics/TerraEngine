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

    DefineType<int8_t>();
    DefineType<uint8_t>();
    DefineType<int16_t>();
    DefineType<uint16_t>();
    DefineType<int32_t>();
    DefineType<uint32_t>();
    DefineType<int64_t>();
    DefineType<uint64_t>();
    DefineType<float>();
    DefineType<double>();

    DefineType<Eigen::Vector2f>()
        .FieldByIndex<float>(0, "x")
        .FieldByIndex<float>(1, "y")
    ;

    DefineType<Eigen::Vector3f>()
        .FieldByIndex<float>(0, "x")
        .FieldByIndex<float>(1, "y")
        .FieldByIndex<float>(2, "z")
    ;

    DefineType<Eigen::Vector4f>()
        .FieldByIndex<float>(0, "x")
        .FieldByIndex<float>(1, "y")
        .FieldByIndex<float>(2, "z")
        .FieldByIndex<float>(3, "w")
    ;


    GDefineMetaClass<FuncConstant>
        ::define("Constant", "Constant")
        .AddOutputPin("Result", &FuncConstant::Result, "Result")
        .AddEmbeddedPin("Value", &FuncConstant::GetValue, &FuncConstant::SetValue, "Value")
    ;

    GDefineMetaClass<FuncConstant2>
        ::define("Constant2", "Constant2")
        .AddOutputPin("Result", &FuncConstant2::Result, "Result")
        .AddEmbeddedPin("Value", &FuncConstant2::GetValue, &FuncConstant2::SetValue, "Value")
    ;

    GDefineMetaClass<FuncConstant3>
        ::define("Constant3", "Constant3")
        .AddOutputPin("Result", &FuncConstant3::Result, "Result")
        .AddEmbeddedPin("Value", &FuncConstant3::GetValue, &FuncConstant3::SetValue, "Value")
    ;

    GDefineMetaClass<FuncConstant4>
        ::define("Constant4", "Constant4")
        .AddOutputPin("Result", &FuncConstant4::Result, "Result")
        .AddEmbeddedPin("Value", &FuncConstant4::GetValue, &FuncConstant4::SetValue, "Value")
    ;

    GDefineMetaClass<FuncBinaryUniversal>
        ::base("Meta")
        .AddInputPin("A", &FuncBinaryUniversal::GetA, &FuncBinaryUniversal::SetA, "A")
        .AddInputPin("B", &FuncBinaryUniversal::GetB, &FuncBinaryUniversal::SetB, "B")
    ;

    GDefineMetaClass<FuncAdd, FuncBinaryUniversal>
        ::define("Add", "Add")
        .AddOutputPin("Result", &FuncAdd::Result, "Result")
    ;

    GDefineMetaClass<FuncMin, FuncBinaryUniversal>
        ::define("Min", "Min")
        .AddOutputPin("Result", &FuncMin::Result, "Result")
    ;

    GDefineMetaClass<FuncMax, FuncBinaryUniversal>
        ::define("Max", "Max")
        .AddOutputPin("Result", &FuncMax::Result, "Result")
    ;
}
