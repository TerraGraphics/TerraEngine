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
    void OnDrawOutputPins(const std::vector<IDraw::Pin>& pins);

private:
    uint8_t m_alpha = 0;
    uintptr_t m_nodeId = 0;
    bool m_existsInputPins = false;

private:
    math::RectF m_nodeRect;
    float m_headerHeight = 0.f;
    float m_maxOutputPinNameWidth = 0.f;
    float m_maxOutputPinNameWidthFrame = 0.f;

private:
    static constexpr const float m_nodePaddingLeft = 8;
    static constexpr const float m_nodePaddingRight = 8;
    static constexpr const float m_nodePaddingTop = 4;
    static constexpr const float m_nodePaddingBottom = 8;

    static constexpr const float m_iconSideSize = 24.f;
};

}
