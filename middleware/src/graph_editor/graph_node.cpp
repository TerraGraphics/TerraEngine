#include "middleware/graph_editor/graph_node.h"

#include <algorithm>
#include <imgui_node_editor.h>

#include "middleware/imgui/widgets.h"


namespace ne = ax::NodeEditor;

GraphPin::GraphPin(bool isInput, uint32_t pinType, GraphNode* node)
    : isInput(isInput)
    , pinType(pinType)
    , node(node) {

}

GraphNode::GraphNode(dg::IReferenceCounters* refCounters, const char* name, uint32_t outputPinType, std::initializer_list<uint32_t> inputPinsType)
    : dg::ObjectBase<dg::IObject>(refCounters)
    , m_name(name)
    , m_outputPin(false, outputPinType, this)
    , m_inputPins(inputPinsType.size(), GraphPin(true, 0, this))
    , m_inputs(inputPinsType.size(), Ref()) {

    uint8_t pinNum = 0;
    for (auto pinType: inputPinsType) {
        m_inputPins[pinNum].pinNum = pinNum;
        m_inputPins[pinNum].pinType = pinType;
        ++pinNum;
    }
}

bool GraphNode::IsFull() const noexcept {
    for (const auto& pin : m_inputs) {
        if ((!pin) || (!pin->IsFull())) {
            return false;
        }
    }

    return true;
}

bool GraphNode::AttachInput(uint8_t number, GraphNode* node) {
    if (number >= m_inputs.size()) {
        return false;
    }

    if (!AttachInputImpl(number, node)) {
        return false;
    }

    m_inputs[number] = node;

    return true;
}

bool GraphNode::DetachInput(uint8_t number) {
    if (number >= m_inputs.size()) {
        return false;
    }

    if (!DetachInputImpl(number)) {
        return false;
    }

    m_inputs[number] = nullptr;

    return true;
}

    float nodePaddingLeft = 8;
    float nodePaddingRight = 8;
    auto iconSize = math::Size(24, 24);
    float leftOffset = nodePaddingLeft;
    ne::PushStyleVar(ne::StyleVar_NodePadding, ImVec4(nodePaddingLeft, 4, nodePaddingRight, 8));

    ne::NodeId id(this);
    ne::BeginNode(id);

    // Header
    ImGui::BeginGroup();
    ImGui::TextUnformatted(m_name);
    auto headerMin = ImGui::GetItemRectMin();
    auto headerMax = ImGui::GetItemRectMax();
    auto headerSize = ImGui::GetItemRectSize();
    ImGui::EndGroup();

    // Input pins
    if (!m_inputPins.empty()) {
        ImGui::BeginGroup();
        for (auto& pin : m_inputPins) {
            ne::BeginPin(ne::PinId(&pin), ne::PinKind::Input);
            NodeIcon(iconSize, IconType::Circle, pin.isConnected, math::Color(0, 255, 0, alpha), math::Color(32, 32, 32, alpha));
            ne::EndPin();
        }
        leftOffset += ImGui::GetItemRectSize().x;
        ImGui::EndGroup();
        ImGui::SameLine();
    }

    ImGui::Dummy(ImVec2(headerSize.x - leftOffset - iconSize.w - nodePaddingRight, 1.0f));

    // Output pin
    if (m_outputPin.pinType != 0) {
        ImGui::SameLine();
        ImGui::BeginGroup();
            ne::BeginPin(ne::PinId(&m_outputPin), ne::PinKind::Output);
            NodeIcon(iconSize, IconType::Circle, m_outputPin.isConnected, math::Color(0, 255, 0, alpha), math::Color(32, 32, 32, alpha));
            ne::EndPin();
        ImGui::EndGroup();
    }

    ne::EndNode();
}
