#pragma once

#include <vector>
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
    void OnFinishDrawNode(bool isValid, void* texBackground, math::SizeF texBackgroundSize);
    void OnDrawInputPins(const std::vector<IDraw::Pin>& pins);
    void OnDrawPreview();
    void OnDrawOutputPins(const std::vector<IDraw::Pin>& pins);

private:
    uintptr_t m_nodeId = 0;
    uint8_t m_alpha = 0;
    uintptr_t m_nodeId = 0;

private:
    float m_headerWidth = 0.f;
    float m_headerBottom = 0.f;
    float m_inputPinsWidth = 0.f;
    float m_outputPinsWidth = 0.f;

private:
    static constexpr const float m_iconSideSize = 24.f;
};

}
