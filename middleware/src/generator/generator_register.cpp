#include "middleware/generator/generator_register.h"

#include <vector>
#include <cstdint>
#include <stdexcept>

#include "core/math/generator_type.h" // IWYU pragma: keep
#include "middleware/gschema/meta/gs_define.h"
#include "middleware/generator/texture/perlin.h"
#include "middleware/generator/texture/spheres.h"
#include "middleware/generator/texture/cylinders.h"
#include "middleware/generator/texture/noise_types.h"
#include "middleware/generator/texture/chess_cubes.h"


DEFINE_TYPES_IMPL(detail::GeneratorFuncs) {
    using namespace gs;

    DefineEnum<NoiseQuality>()
        .AddField(NoiseQuality::BestSpeed, "BestSpeed")
        .AddField(NoiseQuality::Default, "Default")
        .AddField(NoiseQuality::BestQuality, "BestQuality")
    ;

    DefineEnum<CoherentNoise::NoiseType>()
        .AddField(CoherentNoise::NoiseType::OpenSimplex2, "OpenSimplex2")
        .AddField(CoherentNoise::NoiseType::OpenSimplex2S, "OpenSimplex2S")
        .AddField(CoherentNoise::NoiseType::Cellular, "Cellular")
        .AddField(CoherentNoise::NoiseType::Perlin, "Perlin")
        .AddField(CoherentNoise::NoiseType::ValueCubic, "ValueCubic")
        .AddField(CoherentNoise::NoiseType::Value, "Value")
    ;

    DefineEnum<CoherentNoise::FractalType>()
        .AddField(CoherentNoise::FractalType::None, "None")
        .AddField(CoherentNoise::FractalType::FBm, "FBm")
        .AddField(CoherentNoise::FractalType::Ridged, "Ridged")
        .AddField(CoherentNoise::FractalType::PingPong, "PingPong")
    ;

    DefineEnum<CoherentNoise::CellularDistanceFunction>()
        .AddField(CoherentNoise::CellularDistanceFunction::Euclidean, "Euclidean")
        .AddField(CoherentNoise::CellularDistanceFunction::EuclideanSq, "EuclideanSq")
        .AddField(CoherentNoise::CellularDistanceFunction::Manhattan, "Manhattan")
        .AddField(CoherentNoise::CellularDistanceFunction::Hybrid, "Hybrid")
    ;

    DefineEnum<CoherentNoise::CellularReturnType>()
        .AddField(CoherentNoise::CellularReturnType::CellValue, "CellValue")
        .AddField(CoherentNoise::CellularReturnType::Distance, "Distance")
        .AddField(CoherentNoise::CellularReturnType::Distance2, "Distance2")
        .AddField(CoherentNoise::CellularReturnType::Distance2Add, "Distance2Add")
        .AddField(CoherentNoise::CellularReturnType::Distance2Sub, "Distance2Sub")
        .AddField(CoherentNoise::CellularReturnType::Distance2Mul, "Distance2Mul")
        .AddField(CoherentNoise::CellularReturnType::Distance2Div, "Distance2Div")
    ;
}

DEFINE_CLASSES_IMPL(detail::GeneratorFuncs) {
    using namespace gs;

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
