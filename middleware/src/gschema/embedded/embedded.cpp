#include "middleware/gschema/embedded/embedded.h"

#include <vector>
#include <cstdint>
#include <stdexcept>

#include "eigen/core.h"
#include "middleware/gschema/meta/gs_define.h"
#include "middleware/gschema/embedded/func_add.h"
#include "middleware/gschema/embedded/func_min.h"
#include "middleware/gschema/embedded/func_max.h"
#include "middleware/gschema/embedded/func_base.h"
#include "middleware/gschema/embedded/func_constant.h"


DEFINE_TYPES_IMPL(detail::EmbeddedFuncs) {
    using namespace gs;

    DefineType<Eigen::Vector2f>()
        .AddFieldByIndex<float>(0, "x")
        .AddFieldByIndex<float>(1, "y")
    ;

    DefineType<Eigen::Vector3f>()
        .AddFieldByIndex<float>(0, "x")
        .AddFieldByIndex<float>(1, "y")
        .AddFieldByIndex<float>(2, "z")
    ;

    DefineType<Eigen::Vector4f>()
        .AddFieldByIndex<float>(0, "x")
        .AddFieldByIndex<float>(1, "y")
        .AddFieldByIndex<float>(2, "z")
        .AddFieldByIndex<float>(3, "w")
    ;
}

DEFINE_CLASSES_IMPL(detail::EmbeddedFuncs) {
    using namespace gs;

    DefineClass<FuncConstant>("Constant", "Constant")
        .AddOutputPin("Result", &FuncConstant::Result, "Result")
        .AddEmbeddedPinArithmetic("Value", &FuncConstant::GetValue, &FuncConstant::SetValue, "Value")
    ;

    DefineClass<FuncConstant2>("Constant2", "Constant2")
        .AddOutputPin("Result", &FuncConstant2::Result, "Result")
        .AddEmbeddedPinArray("Value", &FuncConstant2::GetValue, &FuncConstant2::SetValue, "Value")
    ;

    DefineClass<FuncConstant3>("Constant3", "Constant3")
        .AddOutputPin("Result", &FuncConstant3::Result, "Result")
        .AddEmbeddedPinArray("Value", &FuncConstant3::GetValue, &FuncConstant3::SetValue, "Value")
    ;

    DefineClass<FuncConstant4>("Constant4", "Constant4")
        .AddOutputPin("Result", &FuncConstant4::Result, "Result")
        .AddEmbeddedPinArray("Value", &FuncConstant4::GetValue, &FuncConstant4::SetValue, "Value")
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
