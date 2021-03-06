#include "core/material/render_target.h"

#include <cstddef>

#include "dg/device.h"
#include "dg/context.h"
#include "core/common/exception.h"


RenderTarget::RenderTarget(const DevicePtr& device, const ContextPtr& context, const SwapChainPtr& swapChain, const std::shared_ptr<MaterialBuilder>& materialBuilder)
    : m_device(device)
    , m_context(context)
    , m_swapChain(swapChain)
    , m_materialBuilder(materialBuilder) {

}

RenderTarget::~RenderTarget() {

}

void RenderTarget::Create(math::Color4f clearColor, uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
    m_targetsIdDirty = true;

    for (size_t i=0; i!=TargetsFormat::MAX_COLOR_TARGETS; ++i) {
        m_clearColors[i] = clearColor;
    }

    dg::FenceDesc fenceDesc;
    fenceDesc.Name = "rt::fence::cpu";
    m_device->CreateFence(fenceDesc, &m_cpuTargetFence);
}

TextureViewPtr RenderTarget::GetColorTexture(uint8_t index) {
    if (index >= TargetsFormat::MAX_COLOR_TARGETS) {
        throw EngineError("wrong index {} for RenderTarget::GetColorTexture, max index is {}", index, TargetsFormat::MAX_COLOR_TARGETS-1);
    }

    if (!m_colorTargets[index]) {
        throw EngineError("wrong index {} for RenderTarget::GetColorTexture: attempted get default texture", index);
    }

    m_targetsIdDirty = true;
    return TextureViewPtr(m_colorTargets[index]->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE));
}

void RenderTarget::SetDefaultColorTarget(uint8_t index, math::Color4f clearColor) {
    m_targetsIdDirty = true;
    m_targets.SetColorTarget(index, m_swapChain->GetDesc().ColorBufferFormat);
    m_colorTargets[index].Release();
    m_clearColors[index] = clearColor;
}

void RenderTarget::SetColorTarget(uint8_t index, dg::TEXTURE_FORMAT format, math::Color4f clearColor, const char* name) {
    m_targetsIdDirty = true;
    m_targets.SetColorTarget(index, format);
    CreateColorTarget(index, format, name);
    m_clearColors[index] = clearColor;
}

TextureViewRaw RenderTarget::GetDepthTargetView() {
    if (m_depthTarget.RawPtr() == nullptr) {
        throw EngineError("RenderTarget::GetDepthTargetView: attempted get default texture");
    }

    return TextureViewPtr(m_depthTarget->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE));
}

void RenderTarget::SetDefaultDepthTarget() {
    m_targetsIdDirty = true;
    m_targets.SetDepthTarget(m_swapChain->GetDesc().DepthBufferFormat);
    m_depthTarget.Release();
}

void RenderTarget::SetDepthTarget(dg::TEXTURE_FORMAT format, const char* name) {
    m_targetsIdDirty = true;
    m_targets.SetDepthTarget(format);
    CreateDepthTarget(format, name);
}

void RenderTarget::CreateColorTarget(uint8_t index, dg::TEXTURE_FORMAT format, const char* name) {
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

    m_colorTargets[index].Release();
    m_device->CreateTexture(desc, nullptr, &m_colorTargets[index]);

    m_colorTargetsView[index] = m_colorTargets[index]->GetDefaultView(dg::TEXTURE_VIEW_RENDER_TARGET);
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
    desc.ClearValue.DepthStencil.Depth = 1.f;
    desc.ClearValue.DepthStencil.Stencil = 0;
    desc.CommandQueueMask = 1;

    m_depthTarget.Release();
    m_device->CreateTexture(desc, nullptr, &m_depthTarget);

    m_depthTagretView = m_depthTarget->GetDefaultView(dg::TEXTURE_VIEW_DEPTH_STENCIL);
}

void RenderTarget::CreateCPUTarget(dg::TEXTURE_FORMAT format, uint32_t width, uint32_t height) {
    dg::TextureDesc desc;
    desc.Name = "rt::color::cpu";
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

    m_cpuTarget.Release();
    m_device->CreateTexture(desc, nullptr, &m_cpuTarget);
}

uint16_t RenderTarget::Update(uint8_t countColorTargets, uint32_t width, uint32_t height) {
    if (m_targets.countColorTargets != countColorTargets) {
        m_targetsIdDirty = true;
        m_targets.SetCountColorTargets(countColorTargets);
    }

    if (m_targetsIdDirty) {
        m_targetsId = m_materialBuilder->CacheTargetsFormat(m_targets);
    }

    bool existsDefaultTarget = false;
    for (uint8_t i=0; i!=countColorTargets; ++i) {
        if (m_colorTargets[i].RawPtr() == nullptr) {
            existsDefaultTarget = true;
            break;
        }
    }

    if ((!existsDefaultTarget) && (m_depthTarget.RawPtr() == nullptr)) {
        existsDefaultTarget = true;
    }

    if (existsDefaultTarget || (width == 0) || (height == 0)) {
        width = m_swapChain->GetDesc().Width;
        height = m_swapChain->GetDesc().Height;
    }

    if ((width == 0) || (height == 0)) {
        return m_targetsId;
    }

    bool needRecreate = ((m_width != width) || (m_height != height));
    m_width = width;
    m_height = height;

    for (uint8_t i=0; i!=countColorTargets; ++i) {
        if (m_colorTargets[i].RawPtr() == nullptr) {
            m_colorTargetsView[i] = m_swapChain->GetCurrentBackBufferRTV();
            continue;
        }

        const auto& desc = m_colorTargets[i]->GetDesc();
        if ((desc.Width != m_width) || (desc.Height != m_height)) {
            CreateColorTarget(i, desc.Format, desc.Name);
        }
    }

    if (m_depthTarget.RawPtr() == nullptr) {
        m_depthTagretView = m_swapChain->GetDepthBufferDSV();
    } else if (needRecreate) {
        const auto& desc = m_depthTarget->GetDesc();
        CreateDepthTarget(desc.Format, desc.Name);
    }

    return m_targetsId;
}

void RenderTarget::Bind() {
    m_context->SetRenderTargets(static_cast<uint32_t>(m_targets.countColorTargets), m_colorTargetsView, m_depthTagretView, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    for (size_t i=0; i!=m_targets.countColorTargets; ++i) {
        m_context->ClearRenderTarget(m_colorTargetsView[i], m_clearColors[i].value, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }
    m_context->ClearDepthStencil(m_depthTagretView, dg::CLEAR_DEPTH_FLAG, 1.0f, 0, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void RenderTarget::CopyColorTarget(uint8_t index, math::Rect rect) {
    if (index >= TargetsFormat::MAX_COLOR_TARGETS) {
        throw EngineError("wrong index {} for RenderTarget::CopyColorTarget, max index is {}", index, TargetsFormat::MAX_COLOR_TARGETS-1);
    }

    if (!m_colorTargets[index]) {
        throw EngineError("wrong index {} for RenderTarget::CopyColorTarget: attempted copy default texture", index);
    }

    if (m_device->GetDeviceCaps().IsGLDevice()) {
        rect.y = m_height - rect.y;
    }

    bool needCreate = true;
    if (m_cpuTarget.RawPtr() != nullptr) {
        const auto& cDesc = m_colorTargets[index]->GetDesc();
        const auto& rtDesc = m_cpuTarget->GetDesc();
        if ((rtDesc.Width == rect.Width()) && (rtDesc.Height == rect.Height()) && (rtDesc.Format == cDesc.Format)) {
            needCreate = false;
        }
    }

    if (needCreate) {
        CreateCPUTarget(m_colorTargets[index]->GetDesc().Format, rect.Width(), rect.Height());
    }

    dg::CopyTextureAttribs cpyAttr;
    cpyAttr.pSrcTexture = m_colorTargets[index];
    cpyAttr.SrcMipLevel = 0;
    cpyAttr.SrcSlice = 0;
    dg::Box box;
    box.MinX = rect.Left();
    box.MaxX = rect.Right();
    box.MinY = rect.Top();
    box.MaxY = rect.Bottom();
    cpyAttr.pSrcBox = &box;
    cpyAttr.SrcTextureTransitionMode = dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;
    cpyAttr.pDstTexture = m_cpuTarget;
    cpyAttr.DstMipLevel = 0;
    cpyAttr.DstSlice = 0;
    cpyAttr.DstX = 0;
    cpyAttr.DstY = 0;
    cpyAttr.DstZ = 0;
    cpyAttr.DstTextureTransitionMode = dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

    m_context->SetRenderTargets(0, nullptr, nullptr, dg::RESOURCE_STATE_TRANSITION_MODE_NONE);
    m_context->CopyTexture(cpyAttr);
    m_context->SignalFence(m_cpuTargetFence, ++m_cpuTargetFenceLast);
}

bool RenderTarget::ReadCPUTarget(uint32_t& result) {
    if ((m_cpuTargetFenceLast == 0) || (m_cpuTargetFenceLast > m_cpuTargetFence->GetCompletedValue())) {
        return false;
    }

    dg::MappedTextureSubresource texData;
    m_context->MapTextureSubresource(m_cpuTarget, 0, 0, dg::MAP_READ, dg::MAP_FLAG_DO_NOT_WAIT, nullptr, texData);
    result = *reinterpret_cast<const uint32_t*>(texData.pData);
    m_context->UnmapTextureSubresource(m_cpuTarget, 0, 0);

    auto format = m_cpuTarget->GetDesc().Format;
    if ((format == dg::TEX_FORMAT_BGRA8_TYPELESS) || (format == dg::TEX_FORMAT_BGRA8_UNORM) || (format == dg::TEX_FORMAT_BGRA8_UNORM_SRGB)) {
        result = math::BGRAToRGBA(result);
    }

    return true;
}
