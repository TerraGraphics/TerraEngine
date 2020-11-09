#include "middleware/generator/gs_register.h"

#include "core/math/generator_type.h" // IWYU pragma: keep
#include "middleware/gscheme/meta/gs_define.h"
#include "middleware/generator/texture/spheres.h"
#include "middleware/generator/texture/cylinders.h"
#include "middleware/generator/texture/chess_cubes.h"
#include "middleware/gscheme/meta/gs_primitive_type.h"
#include "middleware/gscheme/meta/gs_primitive_type_property.h"


REFLECTION_IMPL(gs::GeneratorFuncs) {
    using namespace gs;

    DefineClass<ChessCubes>("ChessCube", "Noise chess cubes")
        .AddOutputPin("Result", &ChessCubes::Result, "Result")
        .AddEmbeddedPin("Frequency", &ChessCubes::GetFrequency, &ChessCubes::SetFrequency, "Frequency")
    ;

    DefineClass<Spheres>("Spheres", "Noise spheres")
        .AddOutputPin("Result", &Spheres::Result, "Result")
        .AddEmbeddedPin("Frequency", &Spheres::GetFrequency, &Spheres::SetFrequency, "Frequency")
    ;

    DefineClass<Cylinders>("Cylinders", "Noise cylinders")
        .AddOutputPin("Result", &Cylinders::Result, "Result")
        .AddEmbeddedPin("Frequency", &Cylinders::GetFrequency, &Cylinders::SetFrequency, "Frequency")
    ;
}
