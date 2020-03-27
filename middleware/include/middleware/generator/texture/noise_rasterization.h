#pragma once

#include "core/dg/dg.h"
#include "core/math/types.h"
#include "middleware/generator/texture/node.h"
#include "middleware/generator/texture/noise_pojection.h"


class NoiseRasterization2D : public GraphNode {
protected:
    NoiseRasterization2D();
    bool AttachInputImpl(uint8_t number, GraphNode* node) override;

public:
    virtual TexturePtr Get() = 0;

protected:
    Noise2D* m_noiseNode = nullptr;
};

class NoiseToTexture : public NoiseRasterization2D {
public:
    NoiseToTexture() = delete;
    NoiseToTexture(DevicePtr& device, ContextPtr& context);
    ~NoiseToTexture();

    TexturePtr Get() override;

private:
    DevicePtr m_device;
    ContextPtr m_context;
    TexturePtr m_texture;
    math::Size m_textureSize = math::Size(256, 256);
    math::RectD m_noiseBound = math::RectD(math::PointD(0, 0), math::PointD(1, 1));
};
