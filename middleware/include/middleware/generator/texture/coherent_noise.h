#pragma once

#include <cstdint>
#define FN_USE_DOUBLES
#include <FastNoise.h>

#include "middleware/graph_editor/graph_node.h"


namespace Diligent {
    class IReferenceCounters;
}
class Noise3D : public GraphNode {
protected:
    Noise3D() = delete;
    Noise3D(dg::IReferenceCounters* refCounters, const char* name);
    bool AttachInputImpl(uint8_t /* number */, GraphNode* /* node */) override { return false; }
    bool DetachInputImpl(uint8_t /* number */) override { return false; }

public:
    static uint32_t OutputTypeID();

    virtual double Get(double x, double y, double z) = 0;
};

class CoherentNoise : public Noise3D {
public:
    enum class NoiseType : uint8_t {
        Value,
        ValueFractal,
        Perlin,
        PerlinFractal,
        Simplex,
        SimplexFractal,
        Cellular,
        WhiteNoise,
        Cubic,
        CubicFractal
    };

    enum class Interpolation : uint8_t {
        Linear,
        Hermite,
        Quintic
    };

    enum class FractalType : uint8_t {
        FBM,
        Billow,
        RigidMulti
    };

    enum class CellularDistanceFunction : uint8_t {
        Euclidean,
        Manhattan,
        Natural
    };

    enum class CellularReturnType : uint8_t {
        CellValue,
        NoiseLookup,
        Distance,
        Distance2,
        Distance2Add,
        Distance2Sub,
        Distance2Mul,
        Distance2Div
    };

public:
    CoherentNoise() = delete;
    CoherentNoise(dg::IReferenceCounters* refCounters);
    static const char* GetName() { return "Coherent noise"; }

public:
    NoiseType GetNoiseType() const;
    void SetNoiseType(NoiseType value);

    Interpolation GetInterpolation() const;
    void SetInterpolation(Interpolation value);

    int GetSeed() const;
    void SetSeed(int value);

    double GetFrequency() const;
    void SetFrequency(double value);

    int GetFractalOctaves() const;
    void SetFractalOctaves(int value);

    double GetFractalLacunarity() const;
    void SetFractalLacunarity(double value);

    double GetFractalGain() const;
    void SetFractalGain(double value);

    FractalType GetFractalType() const;
    void SetFractalType(FractalType value);

    CellularDistanceFunction GetCellularDistanceFunction() const;
    void SetCellularDistanceFunction(CellularDistanceFunction value);

    CellularReturnType GetCellularReturnType() const;
    void SetCellularReturnType(CellularReturnType value);

    FastNoise* GetCellularNoiseLookup() const;
    void SetCellularNoiseLookup(FastNoise* value);

    void GetCellularDistance2Indices(int& cellularDistanceIndex0, int& cellularDistanceIndex1) const;
    void SetCellularDistance2Indices(int cellularDistanceIndex0, int cellularDistanceIndex1);

    double GetCellularJitter() const;
    void SetCellularJitter(double value);

    double GetGradientPerturbAmp() const;
    void SetGradientPerturbAmp(double value);

public:
    double Get(double x, double y, double z) override;
    void DrawGui() override;

private:
    FastNoise m_generator;
};
