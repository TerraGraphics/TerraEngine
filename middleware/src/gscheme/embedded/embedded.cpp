#include "middleware/gscheme/embedded/embedded.h"

#include <cstdint>
#include <stdexcept>

#include "eigen/core.h"
#include "middleware/gscheme/graph/gs_define.h"
#include "middleware/gscheme/embedded/func_add.h"
#include "middleware/gscheme/embedded/func_min.h"
#include "middleware/gscheme/embedded/func_max.h"
#include "middleware/gscheme/embedded/func_base.h"
#include "middleware/gscheme/embedded/func_constant.h"


REFLECTION_IMPL(gs::EmbeddedFuncs) {
    using namespace gs;

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


    DefineClass<FuncConstant>("Constant", "Constant")
        .AddOutputPin("Result", &FuncConstant::Result, "Result")
        .AddEmbeddedPin("Value", &FuncConstant::GetValue, &FuncConstant::SetValue, "Value")
    ;

    DefineClass<FuncConstant2>("Constant2", "Constant2")
        .AddOutputPin("Result", &FuncConstant2::Result, "Result")
        .AddEmbeddedPin("Value", &FuncConstant2::GetValue, &FuncConstant2::SetValue, "Value")
    ;

    DefineClass<FuncConstant3>("Constant3", "Constant3")
        .AddOutputPin("Result", &FuncConstant3::Result, "Result")
        .AddEmbeddedPin("Value", &FuncConstant3::GetValue, &FuncConstant3::SetValue, "Value")
    ;

    DefineClass<FuncConstant4>("Constant4", "Constant4")
        .AddOutputPin("Result", &FuncConstant4::Result, "Result")
        .AddEmbeddedPin("Value", &FuncConstant4::GetValue, &FuncConstant4::SetValue, "Value")
    ;

    DefineBaseClass<FuncBinaryUniversal>("Meta")
        .AddInputPin("A", &FuncBinaryUniversal::GetA, &FuncBinaryUniversal::SetA, "A")
        .AddInputPin("B", &FuncBinaryUniversal::GetB, &FuncBinaryUniversal::SetB, "B")
    ;

    DefineClass<FuncAdd, FuncBinaryUniversal>("Add", "Add")
        .AddOutputPin("Result", &FuncAdd::Result, "Result")
    ;

    DefineClass<FuncMin, FuncBinaryUniversal>("Min", "Min")
        .AddOutputPin("Result", &FuncMin::Result, "Result")
    ;

    DefineClass<FuncMax, FuncBinaryUniversal>("Max", "Max")
        .AddOutputPin("Result", &FuncMax::Result, "Result")
    ;
}
