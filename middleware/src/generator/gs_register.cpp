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
        .AddEmbeddedPinArithmetic("Frequency", &ChessCubes::GetFrequency, &ChessCubes::SetFrequency, "Frequency")
    ;

    DefineClass<Spheres>("Spheres", "Spheres noise")
        .AddOutputPin("Result", &Spheres::Result, "Result")
        .AddEmbeddedPinArithmetic("Frequency", &Spheres::GetFrequency, &Spheres::SetFrequency, "Frequency")
    ;

    DefineClass<Cylinders>("Cylinders", "Cylinders noise")
        .AddOutputPin("Result", &Cylinders::Result, "Result")
        .AddEmbeddedPinArithmetic("Frequency", &Cylinders::GetFrequency, &Cylinders::SetFrequency, "Frequency")
    ;

    DefineClass<Perlin>("Perlin", "Perlin noise")
        .AddOutputPin("Result", &Perlin::Result, "Result")
        .AddEmbeddedPinArithmetic("Frequency", &Perlin::GetFrequency, &Perlin::SetFrequency, "Frequency")
        .AddEmbeddedPinArithmetic("Lacunarity", &Perlin::GetLacunarity, &Perlin::SetLacunarity, "Lacunarity")
        .AddEmbeddedPinArithmetic("Persistence", &Perlin::GetPersistence, &Perlin::SetPersistence, "Persistence")
        .AddEmbeddedPinArithmetic("OctaveCount", &Perlin::GetOctaveCount, &Perlin::SetOctaveCount, "OctaveCount").Min(1).Max(30)
        .AddEmbeddedPinArithmetic("Seed", &Perlin::GetSeed, &Perlin::SetSeed, "Seed")
        .AddEmbeddedPinEnum("Quality", &Perlin::GetQuality, &Perlin::SetQuality, "Quality")
    ;
}
