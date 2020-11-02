#include "middleware/generator/gs_register.h"

#include "core/math/generator_type.h" // IWYU pragma: keep
#include "middleware/gscheme/graph/gs_define.h"
#include "middleware/gscheme/graph/gs_metadata.h"
#include "middleware/generator/texture/spheres.h"
#include "middleware/generator/texture/cylinders.h"
#include "middleware/generator/texture/chess_cubes.h"


REFLECTION_IMPL(gs::GeneratorFuncs) {
    using namespace gs;
    using namespace cpgf;

    GDefineMetaClass<ChessCubes>
        ::define("ChessCube", "Noise chess cubes")
        ._constructor()
        .AddOutputPin("Result", &ChessCubes::Result, "Result")
        .AddEmbeddedPin("Frequency", &ChessCubes::GetFrequency, &ChessCubes::SetFrequency, "Frequency")
    ;

    GDefineMetaClass<Spheres>
        ::define("Spheres", "Noise spheres")
        ._constructor()
        .AddOutputPin("Result", &Spheres::Result, "Result")
        .AddEmbeddedPin("Frequency", &Spheres::GetFrequency, &Spheres::SetFrequency, "Frequency")
    ;

    GDefineMetaClass<Cylinders>
        ::define("Cylinders", "Noise cylinders")
        ._constructor()
        .AddOutputPin("Result", &Cylinders::Result, "Result")
        .AddEmbeddedPin("Frequency", &Cylinders::GetFrequency, &Cylinders::SetFrequency, "Frequency")
    ;
}
