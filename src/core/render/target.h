#pragma once

#include "core/common/dg.h"
#include "core/common/ctor.h"


class Target : Fixed {
public:
    Target() = default;
    Target(uint32_t width, uint32_t height);
    ~Target() = default;

    TextureViewPtr GetColorTexture();

    void CreateColorTarget(DevicePtr& device, const char* name = nullptr);
    void CreateDepthTarget(DevicePtr& device, const char* name = nullptr);
    void SetSize(DevicePtr& device, uint32_t width, uint32_t height);

    void Set(ContextPtr& context, const float* RGBA);

private:
    uint32_t m_width = 1;
    uint32_t m_height = 1;
    TexturePtr m_color;
    TextureViewPtr m_colorTexture;
    TextureViewPtr m_colorRenderTarget;

    TexturePtr m_depth;
    TextureViewPtr m_depthRenderTarget;
};