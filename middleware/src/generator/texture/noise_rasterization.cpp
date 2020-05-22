#include "middleware/generator/texture/noise_rasterization.h"

#include <algorithm>

#include "core/dg/device.h"
#include "core/dg/context.h"
#include "core/common/exception.h"
#include "middleware/imgui/widgets.h"
#include "middleware/graph_editor/graph_node.h"
#include "middleware/generator/texture/texture_consts.h"
#include "middleware/generator/texture/noise_pojection.h"


namespace {
    bool IsCorrectTextureSize(math::Size size) {
        if ((size.w == 0) || (size.h == 0)) {
            return false;
        }

        if ((size.w == 1) && (size.h == 1)) {
            return true;
        }

        if (((size.w % 4) == 0) && ((size.h % 4) == 0)) {
            return true;
        }

        return false;
    }
}


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

uint32_t NoiseRasterization2D::OutputTypeID() {
    return static_cast<uint32_t>(TexturePinType::Texture);
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
    if (!IsCorrectTextureSize(value)) {
        throw EngineError("NoiseToTexture: size must be non-zero and multiple of 4 or equal to 1");
    }
    m_textureSize = value;
    StateChanged();
}

math::RectD NoiseToTexture::GetBound() const {
    return m_noiseBound;
}

void NoiseToTexture::SetBound(math::RectD value) {
    if ((value.Width() <= 0) || (value.Height() <= 0)) {
        throw EngineError("NoiseToTexture: bound need to be above zero");
    }
    m_noiseBound = value;
    StateChanged();
}

NoiseToTexture* NoiseToTexture::SetInputs(Noise2D* input) {
    if (!AttachInput(0, input)) {
        throw EngineError("NoiseToTexture: can't set input number 0");
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

    // TODO: RAII unmap
    m_context->UnmapTextureSubresource(texture, 0, 0);
    m_context->GenerateMips(texture->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE));

    if (size != m_textureSize) {
        m_isCustomDirty = false;
    }

    return texture;
}

void NoiseToTexture::DrawGui() {
    auto tmpSize = m_textureSize;
    if (gui::InputScalarN("Size w/h", tmpSize.values, uint32_t(4), gui::Range<uint32_t>(4, 8192))) {
        if (IsCorrectTextureSize(tmpSize)) {
            m_textureSize = tmpSize;
            StateChanged();
        }
    }
    if (gui::InputScalarN("Bound x/y", m_noiseBound.valuesStart, 0.1, "{:.1f}")) {
        StateChanged();
    }
    if (gui::InputScalarN("Bound w/h", m_noiseBound.valuesSize, 0.1, gui::Range(0.1), "{:.1f}")) {
        StateChanged();
    }
}

void NoiseToTexture::StateChanged() {
    GraphNode::StateChanged();
    m_isCustomDirty = true;
}

bool NoiseToTexture::GetTextureForDraw(math::Size size, TexturePtr& output) {
    if (!IsCorrectTextureSize(size)) {
        throw EngineError("NoiseToTexture: size must be non-zero and multiple of 4 or equal to 1");
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
