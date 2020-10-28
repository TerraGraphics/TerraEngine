#include "middleware/generator/texture/generator2d_to_texture.h"

#include "dg/device.h"
#include "core/engine.h"
#include "core/common/exception.h"
#include "core/material/texture.h"
#include "core/material/texture_manager.h"


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

Generator2dToTexture::Generator2dToTexture() {

}

Generator2dToTexture::~Generator2dToTexture() {
    m_texture.Release();
}

TexturePtr Generator2dToTexture::Result() {
    if (m_texture) {
        m_texture->SetSize(m_textureSize.w, m_textureSize.h);
    } else {
        m_texture = Engine::Get().GetTextureManager()->CreateDynamicTexture(
            dg::TEX_FORMAT_RGBA8_UNORM, m_textureSize.w, m_textureSize.h, "tex::Generator2dToTexture");
    }

    FillTexture(m_texture);

    return m_texture->Get();
}

void Generator2dToTexture::SetTextureSize(const math::Size v) {
    if (!IsCorrectTextureSize(v)) {
        throw EngineError("Generator2dToTexture: texture size must be non-zero and multiple of 4 or equal to 1");
    }
    m_textureSize = v;
}

void Generator2dToTexture::SetGeneratorRect(const math::RectF v) {
    if ((v.Width() <= 0) || (v.Height() <= 0)) {
        throw EngineError("Generator2dToTexture: generator rectangle dimensions must be greater than zero");
    }
    m_generatorRect = v;
}

void Generator2dToTexture::FillTexture(dg::RefCntAutoPtr<DynamicTexture>& texture) const {
    auto lock = texture->Lock(Engine::Get().GetContext());
    double uDelta  = m_generatorRect.Width() / static_cast<double>(lock.width);
    double vDelta  = m_generatorRect.Height() / static_cast<double>(lock.height);
    double v = m_generatorRect.y;

    for (uint32_t y=0; y!=lock.height; ++y) {
        auto* pDest = reinterpret_cast<uint32_t*>(lock.data + lock.stride * y);
        double u = m_generatorRect.x;
        for (uint32_t x=0; x!=lock.width; ++x) {
            double d = std::min(std::max((m_input(u, v) + 1.) * 255. * 0.5, 0.), 255.);
            auto component = static_cast<uint8_t>(std::min(std::max(static_cast<int>(d), 0), 255));
            *pDest = math::Color4(component, component, component).value;

            // Go to the next point.
            ++pDest;
            u += uDelta;
        }
        v += vDelta;
    }
}
