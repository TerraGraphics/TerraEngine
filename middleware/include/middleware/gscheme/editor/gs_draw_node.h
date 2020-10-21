#pragma once

#include <cstdint>
#include <string_view>

#include "core/math/types.h"
#include "middleware/gscheme/graph/gs_draw_interface.h"


namespace gs {

class DrawNode {
public:
    DrawNode() = default;
    ~DrawNode() = default;

    void OnStartDrawNode(uintptr_t id, std::string_view prettyName, uint8_t alpha);
    void OnFinishDrawNode(void* texBackground, math::SizeF texBackgroundSize);
    void OnDrawInputPins(const std::vector<IDraw::Pin>& pins);
    void OnDrawPreview();
    void OnDrawOutputPins(const std::vector<IDraw::Pin>& pins);

private:
    uint8_t m_alpha = 0;
    uintptr_t m_nodeId = 0;

private:
    math::RectF m_nodeRect;
    math::SizeF m_headerSize;

    float m_inputPinsWidth = 0.f;
    float m_outputPinsWidth = 0.f;
    float m_outputPinsLabelWidth = 0.f;

    bool m_addSameLine = false;
private:
    static constexpr const float m_iconSideSize = 24.f;
    static constexpr const math::RectOffsetF m_nodePadding = math::RectOffsetF(/* left */ 8, /* right */ 8, /* top */ 4, /* bottom */ 8);
};

}
