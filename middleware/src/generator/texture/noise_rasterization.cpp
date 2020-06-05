#include "middleware/generator/texture/noise_rasterization.h"

#include <memory>
#include <algorithm>

#include "dg/device.h"
#include "core/engine.h"
#include "core/common/exception.h"
#include "core/material/texture.h"
#include "middleware/imgui/widgets.h"
#include "core/material/texture_manager.h"
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

NoiseToTexture::NoiseToTexture(dg::IReferenceCounters* refCounters)
    : NoiseRasterization2D(refCounters, GetName()) {

}

NoiseToTexture::~NoiseToTexture() {
    m_textureCacheMain.Release();
    m_textureCacheCustom.Release();
}

math::Size NoiseToTexture::GetSize() const {
    return m_textureSize;
}

void NoiseToTexture::SetSize(math::Size value) {
    if (!IsCorrectTextureSize(value)) {
        throw EngineError("NoiseToTexture: size must be non-zero and multiple of 4 or equal to 1");
    }
    m_textureSize = value;
    // no need to change m_isCustomDirty
    GraphNode::StateChanged();
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
    bool isDirty = false;
    if (!m_textureCacheMain) {
        isDirty = true;
        m_textureCacheMain = Engine::Get().GetTextureManager()->CreateDynamicTexture(
            dg::TEX_FORMAT_RGBA8_UNORM, m_textureSize.w, m_textureSize.h, "tex::noise");
    } else {
        isDirty = m_textureCacheMain->SetSize(m_textureSize.w, m_textureSize.h);
    }

    FillTexture(m_textureCacheMain, isDirty || GetIsDirty());
    ResetIsDirty();
    return m_textureCacheMain->Get();
}

TexturePtr NoiseToTexture::GetTexture(math::Size size) {
    if (!IsCorrectTextureSize(size)) {
        throw EngineError("NoiseToTexture: size must be non-zero and multiple of 4 or equal to 1");
    }
    bool isDirty = false;
    if (!m_textureCacheCustom) {
        isDirty = true;
        m_textureCacheCustom = Engine::Get().GetTextureManager()->CreateDynamicTexture(
            dg::TEX_FORMAT_RGBA8_UNORM, size.w, size.h, "tex::noise::custom_size");
    } else {
        isDirty = m_textureCacheCustom->SetSize(size.w, size.h);
    }

    FillTexture(m_textureCacheCustom, isDirty || m_isCustomDirty);
    m_isCustomDirty = false;
    return m_textureCacheCustom->Get();
}

void NoiseToTexture::DrawGui() {
    auto tmpSize = m_textureSize;
    if (gui::InputScalarN("Size w/h", tmpSize.values, uint32_t(4), gui::Range<uint32_t>(4, 8192))) {
        if (IsCorrectTextureSize(tmpSize)) {
            m_textureSize = tmpSize;
            // no need to change m_isCustomDirty
            GraphNode::StateChanged();
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

void NoiseToTexture::FillTexture(dg::RefCntAutoPtr<DynamicTexture>& texture, bool isDirty) {
    if ((m_noiseBound.Width() <= 0) || (m_noiseBound.Height() <= 0)) {
        throw EngineError("NoiseToTexture: invalid param: bound");
    }
    if (m_noiseNode == nullptr) {
        throw EngineError("NoiseToTexture: one of the inputs is empty");
    }
    if (!isDirty) {
        return;
    }

    auto lock = texture->Lock(Engine::Get().GetContext());
    double uDelta  = m_noiseBound.Width() / static_cast<double>(lock.width);
    double vDelta  = m_noiseBound.Height() / static_cast<double>(lock.height);
    double v = m_noiseBound.y;

    for (uint32_t y=0; y!=lock.height; ++y) {
        auto* pDest = reinterpret_cast<uint32_t*>(lock.data + lock.stride * y);
        double u = m_noiseBound.x;
        for (uint32_t x=0; x!=lock.width; ++x) {
            double d = std::min(std::max((m_noiseNode->Get(u, v) + 1.) * 255. * 0.5, 0.), 255.);
            auto component = static_cast<uint8_t>(std::min(std::max(static_cast<int>(d), 0), 255));
            *pDest = math::Color4(component, component, component).value;

            // Go to the next point.
            ++pDest;
            u += uDelta;
        }
        v += vDelta;
    }
}
