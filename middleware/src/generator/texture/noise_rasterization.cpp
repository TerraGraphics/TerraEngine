#include "middleware/generator/texture/noise_rasterization.h"

#include <algorithm>

#include "core/dg/texture.h"
#include "core/common/exception.h"
#include "core/dg/render_device.h"
#include "core/dg/device_context.h"
#include "middleware/imgui/widgets.h"


NoiseRasterization2D::NoiseRasterization2D(dg::IReferenceCounters* refCounters, const char* name)
    : INodePreview(refCounters, name, NoiseRasterization2D::OutputTypeID(), {Noise2D::OutputTypeID()}) {

}

bool NoiseRasterization2D::AttachInputImpl(uint8_t /* number */, GraphNode* node) {
    auto* noiseNode = dynamic_cast<Noise2D*>(node);
    if (noiseNode == nullptr) {
        return false;
    }

    m_noiseNode = noiseNode;
    return true;
}

bool NoiseRasterization2D::DetachInputImpl(uint8_t /* number */) {
    m_noiseNode = nullptr;
    return true;
}

NoiseToTexture::NoiseToTexture(dg::IReferenceCounters* refCounters, DevicePtr& device, ContextPtr& context)
    : NoiseRasterization2D(refCounters, GetName())
    , m_device(device)
    , m_context(context) {

}

NoiseToTexture::~NoiseToTexture() {

}

math::Size NoiseToTexture::GetSize() const {
    return m_textureSize;
}

void NoiseToTexture::SetSize(math::Size value) {
    m_textureSize = value;
    StateChanged();
}

math::RectD NoiseToTexture::GetBound() const {
    return m_noiseBound;
}

void NoiseToTexture::SetBound(math::RectD value) {
    m_noiseBound = value;
    StateChanged();
}

NoiseToTexture* NoiseToTexture::SetInputs(Noise2D* input) {
    if (!AttachInput(0, input)) {
        throw EngineError("NoiseRasterization2D: can't set input number 0");
    }

    return this;
}

TexturePtr NoiseToTexture::Get() {
    return GetTexture(m_textureSize);
}

TexturePtr NoiseToTexture::GetTexture(math::Size size) {
    if ((m_noiseBound.Width() <= 0) || (m_noiseBound.Height() <= 0)) {
        throw EngineError("NoiseToTexture: invalid param: bound");
    }
    if (m_noiseNode == nullptr) {
        throw EngineError("NoiseToTexture: one of the inputs is empty");
    }

    TexturePtr texture;
    bool isChanged = GetTextureForDraw(size, texture);
    if (!isChanged) {
        if (size == m_textureSize) {
            if (!GetIsDirty()) {
                return texture;
            }
        } else {
            if (!m_isCustomDirty) {
                return texture;
            }
        }
    }

    double uDelta  = m_noiseBound.Width() / static_cast<double>(size.w);
    double vDelta  = m_noiseBound.Height() / static_cast<double>(size.h);
    double v = m_noiseBound.y;

    dg::MappedTextureSubresource texData;
    m_context->MapTextureSubresource(texture, 0, 0, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD, nullptr, texData);
    if (texData.pData == nullptr) {
        throw EngineError("NoiseToTexture: failed to lock texture");
    }

    for (uint32_t y=0; y!=size.h; ++y) {
        auto* pDest = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(texData.pData) + texData.Stride * y);
        double u = m_noiseBound.x;
        for (uint32_t x=0; x!=size.w; ++x) {
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
    m_context->UnmapTextureSubresource(texture, 0, 0);
    m_context->GenerateMips(texture->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE));

    if (size != m_textureSize) {
        m_isCustomDirty = false;
    }

    return texture;
}

void NoiseToTexture::DrawGui() {
    if (gui::InputScalarN("Size", m_textureSize.values, 2, gui::Step<uint32_t>(1, 10), gui::Range<uint32_t>(1, 8192))) {
        StateChanged();
    }
    if (gui::InputScalarN("Bound", m_noiseBound.values, 4, gui::Step(0.1, 1.0), "%.1f")) {
        StateChanged();
    }
}

void NoiseToTexture::StateChanged() {
    GraphNode::StateChanged();
    m_isCustomDirty = true;
}

bool NoiseToTexture::GetTextureForDraw(math::Size size, TexturePtr& output) {
    if ((size.w <= 0) || (size.h <= 0)) {
        throw EngineError("NoiseToTexture: invalid param: size");
    }

    output = (size == m_textureSize) ? m_textureCacheMain : m_textureCacheCustom;

    bool needCreateTexture = !output;
    if (!needCreateTexture) {
        needCreateTexture = ((output->GetDesc().Width != size.w) || (output->GetDesc().Height != size.h));
    }

    if (needCreateTexture) {
        dg::TextureDesc desc;
        desc.Name           = "tex::noise";
        desc.Type           = dg::RESOURCE_DIM_TEX_2D;
        desc.Width          = size.w;
        desc.Height         = size.h;
        desc.Format         = dg::TEX_FORMAT_RGBA8_UNORM;
        desc.MipLevels      = 0;
        desc.SampleCount    = 1;
        desc.Usage          = dg::USAGE_DYNAMIC;
        desc.BindFlags      = dg::BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = dg::CPU_ACCESS_WRITE;
        desc.MiscFlags      = dg::MISC_TEXTURE_FLAG_GENERATE_MIPS;

        m_device->CreateTexture(desc, nullptr, &output);
        if (!output) {
            throw EngineError("NoiseToTexture: failed to create texture");
        }

        if (size == m_textureSize) {
            m_textureCacheMain = output;
         } else {
            m_textureCacheCustom = output;
         }
    }

    return needCreateTexture;
}
