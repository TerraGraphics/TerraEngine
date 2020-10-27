#pragma once

#include "dg/dg.h"
#include "core/math/types.h"
#include "core/common/ctor.h"
#include "core/math/generator_type.h"


class DynamicTexture;
class Generator2dToTexture : Fixed {
public:
    Generator2dToTexture();
    ~Generator2dToTexture();

    TexturePtr Result();

    math::Size GetTextureSize() const { return m_textureSize; }
    void SetTextureSize(const math::Size v);

    math::RectF GetGeneratorRect() const { return m_generatorRect; }
    void SetGeneratorRect(const math::RectF v);

    math::Generator2D GetInput() const { return m_input; }
    void SetInput(const math::Generator2D& v) { m_input = v; }

private:
    void FillTexture(dg::RefCntAutoPtr<DynamicTexture>& texture) const;

private:
    dg::RefCntAutoPtr<DynamicTexture> m_texture;
    math::Size m_textureSize = math::Size(128, 128);
    math::RectF m_generatorRect = math::RectF(-16.f, -16.f, 32.f, 32.f);
    math::Generator2D m_input;
};
