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
    ColorTargetDesc(dg::TEXTURE_FORMAT format, const char* name) noexcept
        : type(Type::Custom)
        , format(format)
        , name(name) {

    }

    Type type = Type::Default;
    dg::TEXTURE_FORMAT format = dg::TEXTURE_FORMAT(0);
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
    DepthTargetDesc(dg::TEXTURE_FORMAT format, const char* name) noexcept
        : type(Type::Custom)
        , format(format)
        , name(name) {

    }

    Type type = Type::Default;
    dg::TEXTURE_FORMAT format = dg::TEXTURE_FORMAT(0);
    const char* name = nullptr;
};

struct CPUTargetDesc {
    CPUTargetDesc() noexcept = default;
    CPUTargetDesc(dg::TEXTURE_FORMAT format, uint32_t width, uint32_t height, const char* name) noexcept
        : format(format)
        , width(width)
        , height(height)
        , name(name) {

    }

    bool IsEmpty() const noexcept {
        return ((format == dg::TEXTURE_FORMAT(0)) || (width == 0) || (height == 0));
    }

    dg::TEXTURE_FORMAT format = dg::TEXTURE_FORMAT(0);
    uint32_t width = 0;
    uint32_t height = 0;
    const char* name = nullptr;
};

struct RenderTargetDesc {
    RenderTargetDesc(uint8_t numColorTargets, const ColorTargetDesc* colorTargets, const DepthTargetDesc& depthTarget,
        uint32_t width = 1, uint32_t height = 1)
        : numColorTargets(numColorTargets)
        , colorTargets(colorTargets)
        , depthTarget(depthTarget)
        , width(width)
        , height(height) {

    }

    RenderTargetDesc(uint8_t numColorTargets, const ColorTargetDesc* colorTargets, const DepthTargetDesc& depthTarget,
        CPUTargetDesc&& cpuTarget, uint32_t width = 1, uint32_t height = 1)
        : numColorTargets(numColorTargets)
        , colorTargets(colorTargets)
        , depthTarget(depthTarget)
        , cpuTarget(std::move(cpuTarget))
        , width(width)
        , height(height) {

    }

    uint8_t numColorTargets;
    const ColorTargetDesc* colorTargets;
    const DepthTargetDesc& depthTarget;
    const CPUTargetDesc cpuTarget;

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

    void CopyColorTarget(ContextPtr& context, uint8_t num, uint32_t offsetX, uint32_t offsetY);
    uint32_t ReadCPUTarget(ContextPtr& context);

    TextureViewPtr GetColorTexture(uint8_t num);

private:
    void CreateColorTarget(uint8_t num, dg::TEXTURE_FORMAT format, const char* name);
    void CreateDepthTarget(dg::TEXTURE_FORMAT format, const char* name);
    void CreateCPUTarget(dg::TEXTURE_FORMAT format, uint32_t width, uint32_t height, const char* name);

private:
    bool m_existsDefaultTarget = false;
    uint32_t m_width = 0;
    uint32_t m_height = 0;

    DevicePtr m_device;

    std::vector<TexturePtr> m_colorTargets;
    std::vector<dg::ITextureView*> m_colorViews;
    TexturePtr m_depthTarget;
    dg::ITextureView* m_depthView;
    TexturePtr m_cpuTarget;
};
