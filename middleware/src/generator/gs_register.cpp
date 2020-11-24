#include "middleware/generator/gs_register.h"

#include <cstdint>
#include <stdexcept>

#include "core/math/generator_type.h" // IWYU pragma: keep
#include "middleware/gscheme/meta/gs_define.h"
#include "middleware/generator/texture/perlin.h"
#include "middleware/generator/texture/spheres.h"
#include "middleware/generator/texture/cylinders.h"
#include "middleware/generator/texture/noise_types.h"
#include "middleware/generator/texture/chess_cubes.h"


REFLECTION_IMPL(gs::GeneratorFuncs) {
    using namespace gs;

    DefineEnum<NoiseQuality>()
        .AddField(NoiseQuality::BestSpeed, "BestSpeed")
        .AddField(NoiseQuality::Default, "Default")
        .AddField(NoiseQuality::BestQuality, "BestQuality")
    ;

    DefineClass<ChessCubes>("ChessCube", "Chess cubes noise")
        .AddOutputPin("Result", &ChessCubes::Result, "Result")
        .AddEmbeddedPinPrimitive("Frequency", &ChessCubes::GetFrequency, &ChessCubes::SetFrequency, "Frequency")
    ;

    DefineClass<Spheres>("Spheres", "Spheres noise")
        .AddOutputPin("Result", &Spheres::Result, "Result")
        .AddEmbeddedPinPrimitive("Frequency", &Spheres::GetFrequency, &Spheres::SetFrequency, "Frequency")
    ;

    DefineClass<Cylinders>("Cylinders", "Cylinders noise")
        .AddOutputPin("Result", &Cylinders::Result, "Result")
        .AddEmbeddedPinPrimitive("Frequency", &Cylinders::GetFrequency, &Cylinders::SetFrequency, "Frequency")
    ;

    DefineClass<Perlin>("Perlin", "Perlin noise")
        .AddOutputPin("Result", &Perlin::Result, "Result")
        .AddEmbeddedPinEnum("Quality", &Perlin::GetQuality, &Perlin::SetQuality, "Frequency")
    ;
}
