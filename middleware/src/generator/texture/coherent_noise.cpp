#include "middleware/generator/texture/coherent_noise.h"

#include "core/common/exception.h"
#include "middleware/imgui/widgets.h"


namespace {

static FastNoise::NoiseType ToFastNoise(CoherentNoise::NoiseType value) {
    switch (value) {
    case CoherentNoise::NoiseType::Value:
        return FastNoise::NoiseType::Value;
    case CoherentNoise::NoiseType::ValueFractal:
        return FastNoise::NoiseType::ValueFractal;
    case CoherentNoise::NoiseType::Perlin:
        return FastNoise::NoiseType::Perlin;
    case CoherentNoise::NoiseType::PerlinFractal:
        return FastNoise::NoiseType::PerlinFractal;
    case CoherentNoise::NoiseType::Simplex:
        return FastNoise::NoiseType::Simplex;
    case CoherentNoise::NoiseType::SimplexFractal:
        return FastNoise::NoiseType::SimplexFractal;
    case CoherentNoise::NoiseType::Cellular:
        return FastNoise::NoiseType::Cellular;
    case CoherentNoise::NoiseType::WhiteNoise:
        return FastNoise::NoiseType::WhiteNoise;
    case CoherentNoise::NoiseType::Cubic:
        return FastNoise::NoiseType::Cubic;
    case CoherentNoise::NoiseType::CubicFractal:
        return FastNoise::NoiseType::CubicFractal;
    default:
        throw EngineError("Unknown CoherentNoise::NoiseType value = {}", value);
    }
}

static CoherentNoise::NoiseType FromFastNoise(FastNoise::NoiseType value) {
    switch (value) {
    case FastNoise::NoiseType::Value:
        return CoherentNoise::NoiseType::Value;
    case FastNoise::NoiseType::ValueFractal:
        return CoherentNoise::NoiseType::ValueFractal;
    case FastNoise::NoiseType::Perlin:
        return CoherentNoise::NoiseType::Perlin;
    case FastNoise::NoiseType::PerlinFractal:
        return CoherentNoise::NoiseType::PerlinFractal;
    case FastNoise::NoiseType::Simplex:
        return CoherentNoise::NoiseType::Simplex;
    case FastNoise::NoiseType::SimplexFractal:
        return CoherentNoise::NoiseType::SimplexFractal;
    case FastNoise::NoiseType::Cellular:
        return CoherentNoise::NoiseType::Cellular;
    case FastNoise::NoiseType::WhiteNoise:
        return CoherentNoise::NoiseType::WhiteNoise;
    case FastNoise::NoiseType::Cubic:
        return CoherentNoise::NoiseType::Cubic;
    case FastNoise::NoiseType::CubicFractal:
        return CoherentNoise::NoiseType::CubicFractal;
    default:
        throw EngineError("Unknown FastNoise::NoiseType value = {}", value);
    }
}

static FastNoise::Interp ToFastNoise(CoherentNoise::Interpolation value) {
    switch (value) {
    case CoherentNoise::Interpolation::Linear:
        return FastNoise::Interp::Linear;
    case CoherentNoise::Interpolation::Hermite:
        return FastNoise::Interp::Hermite;
    case CoherentNoise::Interpolation::Quintic:
        return FastNoise::Interp::Quintic;
    default:
        throw EngineError("Unknown CoherentNoise::Interpolation value = {}", value);
    }
}

static CoherentNoise::Interpolation FromFastNoise(FastNoise::Interp value) {
    switch (value) {
    case FastNoise::Interp::Linear:
        return CoherentNoise::Interpolation::Linear;
    case FastNoise::Interp::Hermite:
        return CoherentNoise::Interpolation::Hermite;
    case FastNoise::Interp::Quintic:
        return CoherentNoise::Interpolation::Quintic;
    default:
        throw EngineError("Unknown FastNoise::Interp value = {}", value);
    }
}

static FastNoise::FractalType ToFastNoise(CoherentNoise::FractalType value) {
    switch (value) {
    case CoherentNoise::FractalType::FBM:
        return FastNoise::FractalType::FBM;
    case CoherentNoise::FractalType::Billow:
        return FastNoise::FractalType::Billow;
    case CoherentNoise::FractalType::RigidMulti:
        return FastNoise::FractalType::RigidMulti;
    default:
        throw EngineError("Unknown CoherentNoise::FractalType value = {}", value);
    }
}

static CoherentNoise::FractalType FromFastNoise(FastNoise::FractalType value) {
    switch (value) {
    case FastNoise::FractalType::FBM:
        return CoherentNoise::FractalType::FBM;
    case FastNoise::FractalType::Billow:
        return CoherentNoise::FractalType::Billow;
    case FastNoise::FractalType::RigidMulti:
        return CoherentNoise::FractalType::RigidMulti;
    default:
        throw EngineError("Unknown FastNoise::FractalType value = {}", value);
    }
}

static FastNoise::CellularDistanceFunction ToFastNoise(CoherentNoise::CellularDistanceFunction value) {
    switch (value) {
    case CoherentNoise::CellularDistanceFunction::Euclidean:
        return FastNoise::CellularDistanceFunction::Euclidean;
    case CoherentNoise::CellularDistanceFunction::Manhattan:
        return FastNoise::CellularDistanceFunction::Manhattan;
    case CoherentNoise::CellularDistanceFunction::Natural:
        return FastNoise::CellularDistanceFunction::Natural;
    default:
        throw EngineError("Unknown CoherentNoise::CellularDistanceFunction value = {}", value);
    }
}

static CoherentNoise::CellularDistanceFunction FromFastNoise(FastNoise::CellularDistanceFunction value) {
    switch (value) {
    case FastNoise::CellularDistanceFunction::Euclidean:
        return CoherentNoise::CellularDistanceFunction::Euclidean;
    case FastNoise::CellularDistanceFunction::Manhattan:
        return CoherentNoise::CellularDistanceFunction::Manhattan;
    case FastNoise::CellularDistanceFunction::Natural:
        return CoherentNoise::CellularDistanceFunction::Natural;
    default:
        throw EngineError("Unknown FastNoise::CellularDistanceFunction value = {}", value);
    }
}

static FastNoise::CellularReturnType ToFastNoise(CoherentNoise::CellularReturnType value) {
    switch (value) {
    case CoherentNoise::CellularReturnType::CellValue:
        return FastNoise::CellularReturnType::CellValue;
    case CoherentNoise::CellularReturnType::NoiseLookup:
        return FastNoise::CellularReturnType::NoiseLookup;
    case CoherentNoise::CellularReturnType::Distance:
        return FastNoise::CellularReturnType::Distance;
    case CoherentNoise::CellularReturnType::Distance2:
        return FastNoise::CellularReturnType::Distance2;
    case CoherentNoise::CellularReturnType::Distance2Add:
        return FastNoise::CellularReturnType::Distance2Add;
    case CoherentNoise::CellularReturnType::Distance2Sub:
        return FastNoise::CellularReturnType::Distance2Sub;
    case CoherentNoise::CellularReturnType::Distance2Mul:
        return FastNoise::CellularReturnType::Distance2Mul;
    case CoherentNoise::CellularReturnType::Distance2Div:
        return FastNoise::CellularReturnType::Distance2Div;
    default:
        throw EngineError("Unknown CoherentNoise::CellularReturnType value = {}", value);
    }
}

static CoherentNoise::CellularReturnType FromFastNoise(FastNoise::CellularReturnType value) {
    switch (value) {
    case FastNoise::CellularReturnType::CellValue:
        return CoherentNoise::CellularReturnType::CellValue;
    case FastNoise::CellularReturnType::NoiseLookup:
        return CoherentNoise::CellularReturnType::NoiseLookup;
    case FastNoise::CellularReturnType::Distance:
        return CoherentNoise::CellularReturnType::Distance;
    case FastNoise::CellularReturnType::Distance2:
        return CoherentNoise::CellularReturnType::Distance2;
    case FastNoise::CellularReturnType::Distance2Add:
        return CoherentNoise::CellularReturnType::Distance2Add;
    case FastNoise::CellularReturnType::Distance2Sub:
        return CoherentNoise::CellularReturnType::Distance2Sub;
    case FastNoise::CellularReturnType::Distance2Mul:
        return CoherentNoise::CellularReturnType::Distance2Mul;
    case FastNoise::CellularReturnType::Distance2Div:
        return CoherentNoise::CellularReturnType::Distance2Div;
    default:
        throw EngineError("Unknown FastNoise::CellularReturnType value = {}", value);
    }
}

}

Noise3D::Noise3D(dg::IReferenceCounters* refCounters, const char* name)
    : GraphNode(refCounters, name, Noise3D::OutputTypeID(), {}) {

}

CoherentNoise::CoherentNoise(dg::IReferenceCounters* refCounters)
    : Noise3D(refCounters, GetName()) {

    m_generator.SetNoiseType(FastNoise::PerlinFractal);
}

CoherentNoise::NoiseType CoherentNoise::GetNoiseType() const {
    return FromFastNoise(m_generator.GetNoiseType());
}

void CoherentNoise::SetNoiseType(NoiseType value) {
    m_generator.SetNoiseType(ToFastNoise(value));
    StateChanged();
}

int CoherentNoise::GetSeed() const {
    return m_generator.GetSeed();
}

void CoherentNoise::SetSeed(int value) {
    m_generator.SetSeed(value);
    StateChanged();
}

double CoherentNoise::GetFrequency() const {
    return m_generator.GetFrequency();
}

void CoherentNoise::SetFrequency(double value) {
    m_generator.SetFrequency(value);
    StateChanged();
}

CoherentNoise::Interpolation CoherentNoise::GetInterpolation() const {
    return FromFastNoise(m_generator.GetInterp());
}

void CoherentNoise::SetInterpolation(Interpolation value) {
    m_generator.SetInterp(ToFastNoise(value));
    StateChanged();
}

int CoherentNoise::GetFractalOctaves() const {
    return m_generator.GetFractalOctaves();
}

void CoherentNoise::SetFractalOctaves(int value) {
    m_generator.SetFractalOctaves(value);
    StateChanged();
}

double CoherentNoise::GetFractalLacunarity() const {
    return m_generator.GetFractalLacunarity();
}

void CoherentNoise::SetFractalLacunarity(double value) {
    m_generator.SetFractalLacunarity(value);
    StateChanged();
}

double CoherentNoise::GetFractalGain() const {
    return m_generator.GetFractalGain();
}

void CoherentNoise::SetFractalGain(double value) {
    m_generator.SetFractalGain(value);
    StateChanged();
}

CoherentNoise::FractalType CoherentNoise::GetFractalType() const {
    return FromFastNoise(m_generator.GetFractalType());
}

void CoherentNoise::SetFractalType(FractalType value) {
    m_generator.SetFractalType(ToFastNoise(value));
    StateChanged();
}

CoherentNoise::CellularDistanceFunction CoherentNoise::GetCellularDistanceFunction() const {
    return FromFastNoise(m_generator.GetCellularDistanceFunction());
}

void CoherentNoise::SetCellularDistanceFunction(CellularDistanceFunction value) {
    m_generator.SetCellularDistanceFunction(ToFastNoise(value));
    StateChanged();
}

CoherentNoise::CellularReturnType CoherentNoise::GetCellularReturnType() const {
    return FromFastNoise(m_generator.GetCellularReturnType());
}

void CoherentNoise::SetCellularReturnType(CellularReturnType value) {
    m_generator.SetCellularReturnType(ToFastNoise(value));
    StateChanged();
}

FastNoise* CoherentNoise::GetCellularNoiseLookup() const {
    return m_generator.GetCellularNoiseLookup();
}

void CoherentNoise::SetCellularNoiseLookup(FastNoise* value) {
    m_generator.SetCellularNoiseLookup(value);
    StateChanged();
}

void CoherentNoise::GetCellularDistance2Indices(int& cellularDistanceIndex0, int& cellularDistanceIndex1) const {
    m_generator.GetCellularDistance2Indices(cellularDistanceIndex0, cellularDistanceIndex1);
}

void CoherentNoise::SetCellularDistance2Indices(int cellularDistanceIndex0, int cellularDistanceIndex1) {
    m_generator.SetCellularDistance2Indices(cellularDistanceIndex0, cellularDistanceIndex1);
    StateChanged();
}

double CoherentNoise::GetCellularJitter() const {
    return m_generator.GetCellularJitter();
}

void CoherentNoise::SetCellularJitter(double value) {
    m_generator.SetCellularJitter(value);
    StateChanged();
}

double CoherentNoise::GetGradientPerturbAmp() const {
    return m_generator.GetGradientPerturbAmp();
}

void CoherentNoise::SetGradientPerturbAmp(double value) {
    m_generator.SetGradientPerturbAmp(value);
    StateChanged();
}

double CoherentNoise::Get(double x, double y, double z) {
    return m_generator.GetNoise(x, y, z);
}

void CoherentNoise::DrawGui() {
    static constexpr const std::array<const char*, 10> noiseTypeStr = {
        "Value", "ValueFractal", "Perlin", "PerlinFractal", "Simplex", "SimplexFractal", "Cellular", "WhiteNoise", "Cubic", "CubicFractal"};
    static constexpr const std::array<const char*, 3>  interpolationStr = {
        "Linear", "Hermite", "Quintic"};
    static constexpr const std::array<const char*, 3>  fractalTypeStr = {
        "FBM", "Billow", "RigidMulti"};
    static constexpr const std::array<const char*, 3>  cellularDistanceFunctionStr = {
        "Euclidean", "Manhattan", "Natural"};
    static constexpr const std::array<const char*, 8>  cellularReturnTypeStr = {
        "CellValue", "NoiseLookup", "Distance", "Distance2", "Distance2Add", "Distance2Sub", "Distance2Mul", "Distance2Div"};

    auto noiseType = GetNoiseType();
    if (gui::Combo("Noise type", noiseType, noiseTypeStr)) {
        SetNoiseType(noiseType);
    }

    auto interpolation = GetInterpolation();
    if (gui::Combo("Interpolation", interpolation, interpolationStr)) {
        SetInterpolation(interpolation);
    }

    auto seed = GetSeed();
    if (gui::InputScalar("Seed", seed, 1)) {
        SetSeed(seed);
    }

    auto frequency = GetFrequency();
    if (gui::InputScalar("Frequency", frequency, gui::Step(0.001, 0.01), gui::Range(0.001, .1), "%.3f")) {
        SetFrequency(frequency);
    }

    auto octaves = GetFractalOctaves();
    if (gui::InputScalar("Octaves", octaves, 1)) {
        SetFractalOctaves(octaves);
    }

    auto lacunarity = GetFractalLacunarity();
    if (gui::InputScalar("Lacunarity", lacunarity, gui::Step(0.1, 1.), gui::Range(-10., 10.), "%.1f")) {
        SetFractalLacunarity(lacunarity);
    }

    auto gain = GetFractalGain();
    if (gui::InputScalar("Gain", gain, gui::Step(0.1, 1.), gui::Range(-10., 10.), "%.1f")) {
        SetFractalGain(gain);
    }

    auto fractalType = GetFractalType();
    if (gui::Combo("Fractal type", fractalType, fractalTypeStr)) {
        SetFractalType(fractalType);
    }

    auto cellularDistanceFunction = GetCellularDistanceFunction();
    if (gui::Combo("Cellular distance function", cellularDistanceFunction, cellularDistanceFunctionStr)) {
        SetCellularDistanceFunction(cellularDistanceFunction);
    }

    auto cellularReturnType = GetCellularReturnType();
    if (gui::Combo("Cellular teturn type", cellularReturnType, cellularReturnTypeStr)) {
        SetCellularReturnType(cellularReturnType);
    }

    // GetCellularNoiseLookup
    // GetCellularDistance2Indices

    auto cellularJitter = GetCellularJitter();
    if (gui::InputScalar("Cellular jitter", cellularJitter, gui::Step(0.01, 0.1), gui::Range(-10., 10.), "%.2f")) {
        SetCellularJitter(cellularJitter);
    }

    auto gradientPerturbAmp = GetGradientPerturbAmp();
    if (gui::InputScalar("Gradient perturb amp", gradientPerturbAmp, gui::Step(0.01, 0.1), gui::Range(-10., 10.), "%.2f")) {
        SetGradientPerturbAmp(gradientPerturbAmp);
    }
}
