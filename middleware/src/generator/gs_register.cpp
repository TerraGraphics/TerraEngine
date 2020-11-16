#include "middleware/generator/gs_register.h"

#include <cstdint>
#include <stdexcept>

#include "core/math/generator_type.h" // IWYU pragma: keep
#include "middleware/gscheme/meta/gs_define.h"
#include "middleware/generator/texture/spheres.h"
#include "middleware/generator/texture/cylinders.h"
#include "middleware/generator/texture/chess_cubes.h"


REFLECTION_IMPL(gs::GeneratorFuncs) {
    using namespace gs;

    DefineClass<ChessCubes>("ChessCube", "Noise chess cubes")
        .AddOutputPin("Result", &ChessCubes::Result, "Result")
        .AddEmbeddedPinPrimitive("Frequency", &ChessCubes::GetFrequency, &ChessCubes::SetFrequency, "Frequency")
    ;

    DefineClass<Spheres>("Spheres", "Noise spheres")
        .AddOutputPin("Result", &Spheres::Result, "Result")
        .AddEmbeddedPinPrimitive("Frequency", &Spheres::GetFrequency, &Spheres::SetFrequency, "Frequency")
    ;

    DefineClass<Cylinders>("Cylinders", "Noise cylinders")
        .AddOutputPin("Result", &Cylinders::Result, "Result")
        .AddEmbeddedPinPrimitive("Frequency", &Cylinders::GetFrequency, &Cylinders::SetFrequency, "Frequency")
    ;
}
