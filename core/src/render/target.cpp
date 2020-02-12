#include "core/render/target.h"

#include "core/dg/render_device.h"
#include "core/dg/device_context.h"


Target::Target(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height) {

}

TextureViewPtr Target::GetColorTexture() {
    return m_colorTexture;
}

void Target::CreateColorTarget(DevicePtr& device, const char* name) {
    TexturePtr m_colorTarget;
    dg::TextureDesc desc;
    desc.Name = (name != nullptr) ? name : "rt::color";
    desc.Type = dg::RESOURCE_DIM_TEX_2D;
    desc.Width = m_width;
    desc.Height = m_height;
    desc.Format = dg::TEX_FORMAT_RGBA8_UNORM;
    desc.MipLevels = 1;
    desc.SampleCount = 1;
    desc.Usage = dg::USAGE_DEFAULT;
    desc.BindFlags = dg::BIND_SHADER_RESOURCE | dg::BIND_RENDER_TARGET;
    desc.CPUAccessFlags = dg::CPU_ACCESS_NONE;
    desc.MiscFlags = dg::MISC_TEXTURE_FLAG_NONE;
    desc.ClearValue.Format = desc.Format;
    desc.ClearValue.Color[0] = 0.f;
    desc.ClearValue.Color[1] = 0.f;
    desc.ClearValue.Color[2] = 0.f;
    desc.ClearValue.Color[3] = 1.f;
    desc.CommandQueueMask = 1;
    device->CreateTexture(desc, nullptr, &m_color);

    m_colorTexture = m_color->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    m_colorRenderTarget = m_color->GetDefaultView(dg::TEXTURE_VIEW_RENDER_TARGET);
}

void Target::CreateDepthTarget(DevicePtr& device, const char* name) {
    TexturePtr m_colorTarget;
    dg::TextureDesc desc;
    desc.Name = (name != nullptr) ? name : "rt::depth";
    desc.Type = dg::RESOURCE_DIM_TEX_2D;
    desc.Width = m_width;
    desc.Height = m_height;
    desc.Format = dg::TEX_FORMAT_D32_FLOAT;
    desc.MipLevels = 1;
    desc.SampleCount = 1;
    desc.Usage = dg::USAGE_DEFAULT;
    desc.BindFlags = dg::BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = dg::CPU_ACCESS_NONE;
    desc.MiscFlags = dg::MISC_TEXTURE_FLAG_NONE;
    desc.ClearValue.Format = desc.Format;
    desc.ClearValue.DepthStencil.Depth = 1;
    desc.ClearValue.DepthStencil.Stencil = 0;
    desc.ClearValue.Color[0] = 0.f;
    desc.ClearValue.Color[1] = 0.f;
    desc.ClearValue.Color[2] = 0.f;
    desc.ClearValue.Color[3] = 1.f;
    desc.CommandQueueMask = 1;
    device->CreateTexture(desc, nullptr, &m_depth);

    m_depthRenderTarget = m_depth->GetDefaultView(dg::TEXTURE_VIEW_DEPTH_STENCIL);
}

void Target::SetSize(DevicePtr& device, uint32_t width, uint32_t height) {
    if (((m_width == width) && (m_height == height)) || (width == 0) || (height == 0)) {
        return;
    }
    m_width = width;
    m_height = height;

    if (m_color) {
        CreateColorTarget(device, m_color->GetDesc().Name);
    }
    if (m_depth) {
        Target::CreateDepthTarget(device, m_depth->GetDesc().Name);
    }
}

void Target::Set(ContextPtr& context, const float* RGBA) {
    context->SetRenderTargets(1, &m_colorRenderTarget, m_depthRenderTarget, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    context->ClearRenderTarget(m_colorRenderTarget, RGBA, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    context->ClearDepthStencil(m_depthRenderTarget, dg::CLEAR_DEPTH_FLAG, 1.0f, 0, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}
