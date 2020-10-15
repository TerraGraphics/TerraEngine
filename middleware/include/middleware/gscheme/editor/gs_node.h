#pragma once

#include <cstdint>
#include <string_view>

#include "core/math/types.h"
#include "middleware/gscheme/graph/gs_draw_interface.h"


namespace gs {

class Node {
public:
    Node() = default;
    ~Node() = default;

    void OnStartDrawNode(uintptr_t id, std::string_view prettyName, uint8_t alpha);
    void OnFinishDrawNode(void* texBackground, math::SizeF texBackgroundSize);
    void OnDrawInputPins(const std::vector<IDraw::Pin>& pins);
    void OnDrawOutputPins(const std::vector<IDraw::Pin>& pins);

private:
    uint8_t m_alpha = 0;
    uintptr_t m_nodeId = 0;
    math::RectF m_headerRect;
    bool m_existsInputPins = false;
    float m_maxOutputPinNameWidth = 0.f;
    float m_maxOutputPinNameWidthFrame = 0.f;

    static constexpr const float m_nodePaddingLeft = 8;
    static constexpr const float m_nodePaddingRight = 8;
    static constexpr const float m_nodePaddingTop = 4;
    static constexpr const float m_nodePaddingBottom = 8;
};

}
