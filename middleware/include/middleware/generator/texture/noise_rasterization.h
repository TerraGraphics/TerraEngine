#pragma once

#include "core/dg/dg.h"
#include "core/math/types.h"
#include "middleware/graph_editor/graph_node.h"
#include "middleware/generator/texture/texture_consts.h"
#include "middleware/generator/texture/noise_pojection.h"


class NoiseRasterization2D : public GraphNode {
protected:
    NoiseRasterization2D() = delete;
    NoiseRasterization2D(dg::IReferenceCounters* refCounters);
    bool AttachInputImpl(uint8_t number, GraphNode* node) override;

public:
    static uint32_t TypeID() { return static_cast<uint32_t>(TextureGenTypeIDs::NoiseRasterization2D); }

    virtual TexturePtr Get() = 0;

protected:
    Noise2D* m_noiseNode = nullptr;
};

class NoiseToTexture : public NoiseRasterization2D {
public:
    NoiseToTexture() = delete;
    NoiseToTexture(dg::IReferenceCounters* refCounters, DevicePtr& device, ContextPtr& context);
    ~NoiseToTexture();

    TexturePtr Get() override;

private:
    DevicePtr m_device;
    ContextPtr m_context;
    TexturePtr m_texture;
    math::Size m_textureSize = math::Size(256, 256);
    math::RectD m_noiseBound = math::RectD(math::PointD(0, 0), math::PointD(1000, 1000));
};
