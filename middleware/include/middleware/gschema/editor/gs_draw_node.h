#pragma once

#include <vector>
#include <cstdint>
#include <string_view>

#include "core/math/types.h"
#include "core/common/ctor.h"
#include "middleware/gschema/graph/gs_types_decl.h"
#include "middleware/gschema/graph/gs_draw_interface.h"


namespace cpgf {
    class GVariant;
}

namespace gs {

class DrawPreview;
class DrawNode : Noncopyable {
public:
    DrawNode() = default;
    DrawNode(DrawNode&& o) noexcept;
    DrawNode& operator=(DrawNode&&) noexcept = delete;
    ~DrawNode();

    void OnStartDrawGraph();
    void OnStartDrawNode(uintptr_t id, std::string_view displayName, uint8_t alpha);
    void OnFinishDrawNode(bool isValid, std::string_view errorMessage, void* texBackground, math::SizeF texBackgroundSize);
    void OnDrawInputPins(const std::vector<IDraw::Pin>& pins);
    void OnDrawPreview(TypeId valueTypeId, const cpgf::GVariant& value, uint8_t valueVersion);
    void OnDrawOutputPins(const std::vector<IDraw::Pin>& pins);

private:
    uintptr_t m_nodeId = 0;
    bool m_drawed = false;
    bool m_showPreview = false;
    uint8_t m_alpha = 0;
    DrawPreview* m_preview = nullptr;

private:
    float m_headerWidth = 0.f;
    float m_headerBottom = 0.f;
    float m_inputPinsWidth = 0.f;
    float m_outputPinsWidth = 0.f;

private:
    static constexpr const float m_iconSideSize = 24.f;
    static constexpr const math::SizeF m_previewSize = math::SizeF(128.f);
};

}
