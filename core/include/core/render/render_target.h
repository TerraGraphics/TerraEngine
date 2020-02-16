#pragma once

#include "core/math/basic.h"
#include "core/common/ctor.h"


struct ColorTargetDesc {
    enum class Type : uint8_t {
        Default,
        Custom
    };
    // Default color target
    ColorTargetDesc() noexcept = default;
    // Custom color target
    ColorTargetDesc(const char* name) noexcept
        : type(Type::Custom)
        , name(name) {

    }

    Type type = Type::Default;
    const char* name = nullptr;
};

struct DepthTargetDesc {
    enum class Type : uint8_t {
        Default,
        Custom
    };
    // Default depth target
    DepthTargetDesc() noexcept = default;
    // Custom depth target
    DepthTargetDesc(const char* name) noexcept
        : type(Type::Custom)
        , name(name) {

    }

    Type type = Type::Default;
    const char* name = nullptr;
};

struct RenderTargetDesc {
    RenderTargetDesc(uint8_t numColorTargets, const ColorTargetDesc* colorTargets, const DepthTargetDesc& depthTarget, uint32_t width = 1, uint32_t height = 1)
        : numColorTargets(numColorTargets)
        , colorTargets(colorTargets)
        , depthTarget(depthTarget)
        , width(width)
        , height(height) {

    }

    uint8_t numColorTargets;
    const ColorTargetDesc* colorTargets;
    const DepthTargetDesc& depthTarget;

    uint32_t width = 1;
    uint32_t height = 1;
};

class RenderTarget : Fixed {
public:
    RenderTarget() = default;
    ~RenderTarget() = default;

    void Create(const DevicePtr& device);
    void Create(const DevicePtr& device, RenderTargetDesc&& desc);
    void Update(SwapChainPtr& swapChain, uint32_t width = 0, uint32_t height = 0);
    void Bind(ContextPtr& context, const math::Color4f& clearColor);

    TextureViewPtr GetColorTexture(uint8_t num);

private:
    void CreateColorTarget(uint8_t num, const char* name);
    void CreateDepthTarget(const char* name);

private:
    bool m_existsDefaultTarget = false;
    uint32_t m_width = 0;
    uint32_t m_height = 0;

    DevicePtr m_device;

    std::vector<TexturePtr> m_colorTargets;
    std::vector<dg::ITextureView*> m_colorViews;
    TexturePtr m_depthTarget;
    dg::ITextureView* m_depthView;
};
