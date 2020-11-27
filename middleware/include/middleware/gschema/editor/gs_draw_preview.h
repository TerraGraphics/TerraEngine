#pragma once

#include <cstdint>

#include "dg/dg.h"
#include "core/math/types.h"
#include "core/common/ctor.h"
#include "core/math/generator_type_fwd.h"
#include "middleware/gschema/graph/gs_types_decl.h"


namespace cpgf {
    class GVariant;
}

class Generator2dToTexture;
namespace gs {

class DrawPreview : Fixed {
public:
    DrawPreview();
    ~DrawPreview();

    void Reset();
    void Draw(TypeId typeId, const cpgf::GVariant& value, uint8_t valueVersion, math::RectOffsetF margin, math::SizeF drawSize);

private:
    bool IsNeedUpdateTexture(uint8_t valueVersion);
    void FillTexture(const math::Generator2D& v);

private:
    uint8_t m_frameCounter = 0;
    uint8_t m_valueVersion = 0;
    TextureViewPtr m_texture;
    Generator2dToTexture* m_generator = nullptr;
};

}
