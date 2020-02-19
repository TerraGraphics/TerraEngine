#include "core/render/render_target.h"

#include "core/common/exception.h"
#include "core/dg/render_device.h"
#include "core/dg/device_context.h"


void RenderTarget::Create(const DevicePtr& device) {
    ColorTargetDesc colorTargets[] = { ColorTargetDesc() };
    Create(device, RenderTargetDesc(_countof(colorTargets), colorTargets, DepthTargetDesc()));
}

void RenderTarget::Create(const DevicePtr& device, RenderTargetDesc&& desc) {
    if (!m_colorTargets.empty()) {
        throw EngineError("double create for RenderTarget");
    }

    m_width = desc.width;
    m_height = desc.height;
    m_device = device;

    TexturePtr defColorTarget;
    m_colorTargets.resize(desc.numColorTargets, defColorTarget);
    m_colorViews.resize(desc.numColorTargets, nullptr);
    for (uint8_t i=0; i!=desc.numColorTargets; ++i) {
        if (desc.colorTargets[i].type == ColorTargetDesc::Type::Custom) {
            CreateColorTarget(i, desc.colorTargets[i].format, desc.colorTargets[i].name);
        } else {
            m_existsDefaultTarget = true;
        }
    }

    if (desc.depthTarget.type == DepthTargetDesc::Type::Custom) {
        CreateDepthTarget(desc.depthTarget.format, desc.depthTarget.name);
    } else {
        m_existsDefaultTarget = true;
    }

    if (!desc.cpuTarget.IsEmpty()) {
        CreateCPUTarget(desc.cpuTarget.format, desc.cpuTarget.width, desc.cpuTarget.height, desc.cpuTarget.name);

        dg::FenceDesc fenceDesc;
        fenceDesc.Name = "rt::fence::cpu";
        m_device->CreateFence(fenceDesc, &m_cpuTargetFence);
    }
}

void RenderTarget::CreateColorTarget(uint8_t num, dg::TEXTURE_FORMAT format, const char* name) {
    dg::TextureDesc desc;
    desc.Name = (name != nullptr) ? name : "rt::color";
    desc.Type = dg::RESOURCE_DIM_TEX_2D;
    desc.Width = m_width;
    desc.Height = m_height;
    desc.Format = (format == dg::TEXTURE_FORMAT(0)) ? dg::TEX_FORMAT_RGBA8_UNORM : format;
    desc.MipLevels = 1;
    desc.SampleCount = 1;
    desc.Usage = dg::USAGE_DEFAULT;
    desc.BindFlags = dg::BIND_SHADER_RESOURCE | dg::BIND_RENDER_TARGET;
    desc.CPUAccessFlags = dg::CPU_ACCESS_NONE;
    desc.MiscFlags = dg::MISC_TEXTURE_FLAG_NONE;
    desc.CommandQueueMask = 1;
    m_device->CreateTexture(desc, nullptr, &m_colorTargets[num]);

    m_colorViews[num] = m_colorTargets[num]->GetDefaultView(dg::TEXTURE_VIEW_RENDER_TARGET);
}

void RenderTarget::CreateDepthTarget(dg::TEXTURE_FORMAT format, const char* name) {
    dg::TextureDesc desc;
    desc.Name = (name != nullptr) ? name : "rt::depth";
    desc.Type = dg::RESOURCE_DIM_TEX_2D;
    desc.Width = m_width;
    desc.Height = m_height;
    desc.Format = (format == dg::TEXTURE_FORMAT(0)) ? dg::TEX_FORMAT_D32_FLOAT : format;
    desc.MipLevels = 1;
    desc.SampleCount = 1;
    desc.Usage = dg::USAGE_DEFAULT;
    desc.BindFlags = dg::BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = dg::CPU_ACCESS_NONE;
    desc.MiscFlags = dg::MISC_TEXTURE_FLAG_NONE;
    desc.ClearValue.Format = desc.Format;
    desc.ClearValue.DepthStencil.Depth = 1;
    desc.ClearValue.DepthStencil.Stencil = 0;
    desc.CommandQueueMask = 1;
    m_device->CreateTexture(desc, nullptr, &m_depthTarget);

    m_depthView = m_depthTarget->GetDefaultView(dg::TEXTURE_VIEW_DEPTH_STENCIL);
}

void RenderTarget::CreateCPUTarget(dg::TEXTURE_FORMAT format, uint32_t width, uint32_t height, const char* name) {
    dg::TextureDesc desc;
    desc.Name = (name != nullptr) ? name : "rt::color::cpu";
    desc.Type = dg::RESOURCE_DIM_TEX_2D;
    desc.Width = width;
    desc.Height = height;
    desc.Format = format;
    desc.MipLevels = 1;
    desc.SampleCount = 1;
    desc.Usage = dg::USAGE_STAGING;
    desc.BindFlags = dg::BIND_NONE;
    desc.CPUAccessFlags = dg::CPU_ACCESS_READ;
    desc.MiscFlags = dg::MISC_TEXTURE_FLAG_NONE;
    desc.CommandQueueMask = 1;
    m_device->CreateTexture(desc, nullptr, &m_cpuTarget);
}

void RenderTarget::Update(SwapChainPtr& swapChain, uint32_t width, uint32_t height) {
    if (m_existsDefaultTarget || (width == 0) || (height == 0)) {
        width = swapChain->GetDesc().Width;
        height = swapChain->GetDesc().Height;
    }

    if ((width == 0) || (height == 0)) {
        return;
    }

    bool needRecreate = ((m_width != width) || (m_height != height));
    m_width = width;
    m_height = height;

    uint8_t i = 0;
    for (auto& ct : m_colorTargets) {
        if (ct) {
            if (needRecreate) {
                auto& desc = ct->GetDesc();
                CreateColorTarget(i, desc.Format, desc.Name);
            }
        } else {
            m_colorViews[i] = swapChain->GetCurrentBackBufferRTV();
        }
        ++i;
    }

    if (m_depthTarget) {
        if (needRecreate) {
            auto& desc = m_depthTarget->GetDesc();
            CreateDepthTarget(desc.Format, desc.Name);
        }
    } else {
        m_depthView = swapChain->GetDepthBufferDSV();
    }
}

void RenderTarget::Bind(ContextPtr& context, const math::Color4f& clearColor) {
    context->SetRenderTargets(static_cast<uint32_t>(m_colorViews.size()), m_colorViews.data(), m_depthView, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    for (auto* view : m_colorViews) {
        context->ClearRenderTarget(view, clearColor.value, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }
    context->ClearDepthStencil(m_depthView, dg::CLEAR_DEPTH_FLAG, 1.0f, 0, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void RenderTarget::CopyColorTarget(ContextPtr& context, uint8_t num, uint32_t offsetX, uint32_t offsetY) {
    if (num >= m_colorTargets.size()) {
        throw EngineError("wrong number {} for RenderTarget::CopyColorTarget", num);
    }
    if (!m_cpuTarget) {
        throw EngineError("wrong call RenderTarget::CopyColorTarget: cpu texture is uninitialized");
    }

    dg::CopyTextureAttribs cpyAttr;
    cpyAttr.pSrcTexture = m_colorTargets[num];
    cpyAttr.SrcMipLevel = 0;
    cpyAttr.SrcSlice = 0;
    dg::Box box;
    box.MinX = offsetX;
    box.MaxX = offsetX + m_cpuTarget->GetDesc().Width;
    box.MinY = offsetY;
    box.MaxY = offsetY + m_cpuTarget->GetDesc().Height;
    cpyAttr.pSrcBox = &box;
    cpyAttr.SrcTextureTransitionMode = dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;
    cpyAttr.pDstTexture = m_cpuTarget;
    cpyAttr.DstMipLevel = 0;
    cpyAttr.DstSlice = 0;
    cpyAttr.DstX = 0;
    cpyAttr.DstY = 0;
    cpyAttr.DstZ = 0;
    cpyAttr.DstTextureTransitionMode = dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

    context->CopyTexture(cpyAttr);
    context->SignalFence(m_cpuTargetFence, ++m_cpuTargetFenceLast);
}

std::pair<uint32_t, bool> RenderTarget::ReadCPUTarget(ContextPtr& context) {
    if (!m_cpuTarget) {
        throw EngineError("wrong call RenderTarget::ReadCPUTarget: cpu texture is uninitialized");
    }

    if (m_cpuTargetFenceLast > m_cpuTargetFence->GetCompletedValue()) {
        return std::make_pair(0, false);
    }

    dg::MappedTextureSubresource texData;
    context->MapTextureSubresource(m_cpuTarget, 0, 0, dg::MAP_READ, dg::MAP_FLAG_DO_NOT_WAIT, nullptr, texData);
    uint32_t result = *reinterpret_cast<const uint32_t*>(texData.pData);
    context->UnmapTextureSubresource(m_cpuTarget, 0, 0);

    return std::make_pair(result, true);
}

TextureViewPtr RenderTarget::GetColorTexture(uint8_t num) {
    if (num >= m_colorTargets.size()) {
        throw EngineError("wrong number {} for RenderTarget::GetColorTexture", num);
    }

    if (!m_colorTargets[num]) {
        throw EngineError("wrong number {} for RenderTarget::GetColorTexture: attempted get default texture", num);
    }

    return TextureViewPtr(m_colorTargets[num]->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE));
}
