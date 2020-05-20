#pragma once

#include <cstdint>

#include "core/dg/dg.h"
#include "core/math/types.h"
#include "core/common/ctor.h"


class RenderTarget : Fixed {
public:
    RenderTarget();
    ~RenderTarget();

    void Create(const DevicePtr& device, const ContextPtr& context, math::Color4f clearColor, uint32_t width, uint32_t height);

    TextureViewPtr GetColorTexture(uint8_t index);
    void SetDefaultColorTarget(uint8_t index, math::Color4f clearColor = math::Color4f(1.f));
    void SetColorTarget(uint8_t index, dg::TEXTURE_FORMAT format, math::Color4f clearColor = math::Color4f(1.f), const char* name = nullptr);

    TextureViewRaw GetDepthTargetView();
    void SetDefaultDepthTarget();
    void SetDepthTarget(dg::TEXTURE_FORMAT format, const char* name = nullptr);

    void Update(SwapChainPtr& swapChain, uint8_t countColorTargets = 1, uint32_t width = 0, uint32_t height = 0);
    void Bind();

    void CopyColorTarget(uint8_t index, math::Rect rect);
    bool ReadCPUTarget(uint32_t& result);

private:
    void CreateColorTarget(uint8_t index, dg::TEXTURE_FORMAT format, const char* name);
    void CreateDepthTarget(dg::TEXTURE_FORMAT format, const char* name);
    void CreateCPUTarget(dg::TEXTURE_FORMAT format, uint32_t width, uint32_t height);

private:
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    uint8_t m_countColorTargets = 0;

    DevicePtr m_device;
    ContextPtr m_context;

    // Color target
    static constexpr const uint8_t MAX_COLOR_TARGETS = 2;
    TexturePtr m_colorTargets[MAX_COLOR_TARGETS];
    TextureViewRaw m_colorTargetsView[MAX_COLOR_TARGETS];
    math::Color4f m_clearColors[MAX_COLOR_TARGETS];

    // Depth target
    TexturePtr m_depthTarget;
    TextureViewRaw m_depthTagretView;

    // Cpu target
    TexturePtr m_cpuTarget;
    FencePtr m_cpuTargetFence;
    uint64_t m_cpuTargetFenceLast = 0;
};
