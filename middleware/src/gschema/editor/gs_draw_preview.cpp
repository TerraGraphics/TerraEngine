#include "middleware/gschema/editor/gs_draw_preview.h"

#include "dg/device.h"
#include "eigen/core.h"
#include "cpgf/variant.h"
#include "core/common/exception.h"
#include "middleware/imgui/image.h"
#include "middleware/generator/texture/section_plane.h"
#include "middleware/generator/texture/generator2d_to_texture.h"


namespace gs {

DrawPreview::DrawPreview() {

}

DrawPreview::~DrawPreview() {
    if (m_texture.RawPtr() != nullptr) {
        m_texture.Release();
    }
    if (m_generator != nullptr) {
        delete m_generator;
    }
}

void DrawPreview::Reset() {
    m_frameCounter = 0;
    m_valueVersion = 0;
    if (m_texture.RawPtr() != nullptr) {
        m_texture.Release();
    }
}


void DrawPreview::Draw(TypeId typeId, const cpgf::GVariant& value, uint8_t valueVersion, gui::ImageStyle& style, math::SizeF drawSize) {
    if (typeId == TypeId::Float) {
        const auto tmp = cpgf::fromVariant<float>(value);
        style.color.red = math::Color4::ConvertComponent(tmp);
        style.color.green = 0;
        style.color.blue = 0;
        gui::Image(drawSize, style);
    } else if (typeId == TypeId::Vector2f) {
        const auto tmp = cpgf::fromVariant<Eigen::Vector2f>(value);
        style.color.red = math::Color4::ConvertComponent(tmp[0]);
        style.color.green = math::Color4::ConvertComponent(tmp[1]);
        style.color.blue = 0;
        gui::Image(drawSize, style);
    } else if (typeId == TypeId::Vector3f) {
        const auto tmp = cpgf::fromVariant<Eigen::Vector3f>(value);
        style.color.red = math::Color4::ConvertComponent(tmp[0]);
        style.color.green = math::Color4::ConvertComponent(tmp[1]);
        style.color.blue = math::Color4::ConvertComponent(tmp[2]);
        gui::Image(drawSize, style);
    } else if (typeId == TypeId::Vector4f) {
        const auto tmp = cpgf::fromVariant<Eigen::Vector4f>(value);
        style.color.red = math::Color4::ConvertComponent(tmp[0]);
        style.color.green = math::Color4::ConvertComponent(tmp[1]);
        style.color.blue = math::Color4::ConvertComponent(tmp[2]);
        gui::Image(drawSize, style);
    } else if (typeId == TypeId::Generator2d) {
        if (IsNeedUpdateTexture(valueVersion)) {
            const auto tmp = cpgf::fromVariant<math::Generator2D>(value);
            FillTexture(tmp);
        }
        gui::Image(drawSize, m_texture, style);
    } else if (typeId == TypeId::Generator3d) {
        if (IsNeedUpdateTexture(valueVersion)) {
            const auto tmp = cpgf::fromVariant<math::Generator3D>(value);
            auto sPlane = SectionPlaneX0Y();
            sPlane.SetInput(tmp);
            FillTexture(sPlane.Result());
        }
        gui::Image(drawSize, m_texture, style);
    } else {
        throw EngineError("gs::DrawPreview::Draw: unknown value type (id = {})", typeId);
    }
}

bool DrawPreview::IsNeedUpdateTexture(uint8_t valueVersion) {
    if (m_texture.RawPtr() == nullptr) {
        m_frameCounter = 0;
        m_valueVersion = valueVersion;
        return true;
    }

    if (valueVersion == m_valueVersion) {
        return false;
    }

    ++m_frameCounter;
    if (m_frameCounter == 50) {
        m_frameCounter = 0;
        m_valueVersion = valueVersion;
        return true;
    }

    return false;
}

void DrawPreview::FillTexture(const math::Generator2D& v) {
    if (m_generator == nullptr) {
        m_generator = new Generator2dToTexture();
    }

    m_generator->SetInput(v);
    m_texture = m_generator->Result()->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
}

}
