#pragma once

#include <cstdint>

#include "core/dg/dg.h"
#include "core/math/types.h"
#include "middleware/graph_editor/graph_node_preview.h"


namespace Diligent {
    class IReferenceCounters;
}
class Noise2D;
class GraphNode;
class NoiseRasterization2D : public INodePreview {
protected:
    NoiseRasterization2D() = delete;
    NoiseRasterization2D(dg::IReferenceCounters* refCounters, const char* name);
    bool AttachInputImpl(uint8_t number, GraphNode* node) override;
    bool DetachInputImpl(uint8_t number) override;

public:
    static uint32_t OutputTypeID();

    virtual TexturePtr Get() = 0;

protected:
    Noise2D* m_noiseNode = nullptr;
};

class DynamicTexture;
class NoiseToTexture : public NoiseRasterization2D {
public:
    NoiseToTexture() = delete;
    NoiseToTexture(dg::IReferenceCounters* refCounters);
    ~NoiseToTexture();
    static const char* GetName() { return "Noise to texture"; }

public:
    math::Size GetSize() const;
    void SetSize(math::Size value);
    math::RectD GetBound() const;
    void SetBound(math::RectD value);
    NoiseToTexture* SetInputs(Noise2D* input);

public:
    TexturePtr Get() override;
    TexturePtr GetTexture(math::Size size) override;
    void DrawGui() override;

protected:
    void StateChanged() override;

private:
    void FillTexture(dg::RefCntAutoPtr<DynamicTexture>& texture, bool isDirty);

private:
    dg::RefCntAutoPtr<DynamicTexture> m_textureCacheMain;
    dg::RefCntAutoPtr<DynamicTexture> m_textureCacheCustom;
    math::Size m_textureSize = math::Size(256, 256);
    math::RectD m_noiseBound = math::RectD(math::PointD(0, 0), math::PointD(100, 100));
    bool m_isCustomDirty = true;
};
