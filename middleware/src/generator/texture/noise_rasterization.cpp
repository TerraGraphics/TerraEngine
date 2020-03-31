#include "middleware/generator/texture/noise_rasterization.h"

#include <algorithm>

#include "core/dg/texture.h"
#include "core/common/exception.h"
#include "core/dg/render_device.h"
#include "core/dg/device_context.h"


NoiseRasterization2D::NoiseRasterization2D(dg::IReferenceCounters* refCounters)
    : GraphNode(refCounters, Noise3D::OutputTypeID(), {Noise2D::OutputTypeID()}) {

}

bool NoiseRasterization2D::AttachInputImpl(uint8_t /* number */, GraphNode* node) {
    auto* noiseNode = dynamic_cast<Noise2D*>(node);
    if (noiseNode == nullptr) {
        return false;
    }

    m_noiseNode = noiseNode;
    return true;
}

NoiseToTexture::NoiseToTexture(dg::IReferenceCounters* refCounters, DevicePtr& device, ContextPtr& context)
    : NoiseRasterization2D(refCounters)
    , m_device(device)
    , m_context(context) {

}

NoiseToTexture::~NoiseToTexture() {

}

TexturePtr NoiseToTexture::Get() {
    if ((m_noiseBound.Width() <= 0) ||
        (m_noiseBound.Height() <= 0) ||
        (m_textureSize.w <= 0) ||
        (m_textureSize.h <= 0) ||
        (m_noiseNode == nullptr)) {
        throw EngineError("NoiseToTexture: invalid params");
    }

    bool needCreateTexture = !m_texture;
    if (!needCreateTexture) {
        needCreateTexture = ((m_texture->GetDesc().Width != m_textureSize.w) || (m_texture->GetDesc().Height != m_textureSize.h));
    }

    if (needCreateTexture) {
        dg::TextureDesc desc;
        desc.Name           = "tex::noise";
        desc.Type           = dg::RESOURCE_DIM_TEX_2D;
        desc.Width          = m_textureSize.w;
        desc.Height         = m_textureSize.h;
        desc.Format         = dg::TEX_FORMAT_RGBA8_UNORM;
        desc.MipLevels      = 0;
        desc.SampleCount    = 1;
        desc.Usage          = dg::USAGE_DYNAMIC;
        desc.BindFlags      = dg::BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = dg::CPU_ACCESS_WRITE;
        desc.MiscFlags      = dg::MISC_TEXTURE_FLAG_GENERATE_MIPS;

        m_device->CreateTexture(desc, nullptr, &m_texture);
        if (!m_texture) {
            throw EngineError("NoiseToTexture: failed to create texture");
        }
    }

    double uDelta  = m_noiseBound.Width() / static_cast<double>(m_textureSize.w);
    double vDelta  = m_noiseBound.Height() / static_cast<double>(m_textureSize.h);
    double v = m_noiseBound.y;

    dg::MappedTextureSubresource texData;
    m_context->MapTextureSubresource(m_texture, 0, 0, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD, nullptr, texData);
    if (texData.pData == nullptr) {
        throw EngineError("NoiseToTexture: failed to map texture");
    }

    for (uint32_t y=0; y!=m_textureSize.h; ++y) {
        auto* pDest = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(texData.pData) + texData.Stride * y);
        double u = m_noiseBound.x;
        for (uint32_t x=0; x!=m_textureSize.w; ++x) {
            double d = std::min(std::max((m_noiseNode->Get(u, v) + 1.) * 255. * 0.5, 0.), 255.);
            auto component = static_cast<uint8_t>(std::min(std::max(static_cast<int>(d), 0), 255));
            *pDest = math::Color4(component, component, component).value;

            // Go to the next point.
            ++pDest;
            u += uDelta;
        }
        v += vDelta;
    }

    // TODO RAII unmap
    m_context->UnmapTextureSubresource(m_texture, 0, 0);
    m_context->GenerateMips(m_texture->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE));

    return m_texture;
}
