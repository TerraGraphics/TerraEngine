#pragma once

#include <vector>
#include <cstdint>
#include <string_view>

#include "dg/dg.h"
#include "core/math/types.h"
#include "core/common/ctor.h"
#include "core/math/generator_type_fwd.h"
#include "middleware/gscheme/graph/gs_types_decl.h"
#include "middleware/gscheme/graph/gs_draw_interface.h"


namespace cpgf {
    class GVariant;
}

class Generator2dToTexture;
namespace gs {

class DrawNode : Noncopyable {
public:
    DrawNode() = default;
    DrawNode(DrawNode&& o) noexcept;
    DrawNode& operator=(DrawNode&&) noexcept = delete;
    ~DrawNode();

    void OnStartDrawGraph();
    void OnStartDrawNode(uintptr_t id, std::string_view prettyName, uint8_t alpha);
    void OnFinishDrawNode(bool isValid, std::string_view errorMessage, void* texBackground, math::SizeF texBackgroundSize);
    void OnDrawInputPins(const std::vector<IDraw::Pin>& pins);
    void OnDrawPinPreview(TypeId typeId, const cpgf::GVariant& value);
    void OnDrawOutputPins(const std::vector<IDraw::Pin>& pins);

private:
    void FillTexture(const math::Generator2D& v);

private:
    uintptr_t m_nodeId = 0;
    bool m_drawed = false;
    bool m_showPinPreview = false;
    uint8_t m_alpha = 0;
    uint8_t m_frameNum = 0;
    TextureViewPtr m_texture;
    Generator2dToTexture* m_textureGenerator = nullptr;

private:
    float m_headerWidth = 0.f;
    float m_headerBottom = 0.f;
    float m_inputPinsWidth = 0.f;
    float m_outputPinsWidth = 0.f;

private:
    static constexpr const float m_iconSideSize = 24.f;
    static constexpr const math::SizeF m_pinPreviewSize = math::SizeF(128.f);
};

}
