#include "middleware/graph_editor/graph_node.h"

#include <array>
#include <memory>
#include <imgui.h>
#include <ext/alloc_traits.h>
#include <imgui_node_editor.h>

#include "core/math/types.h"
#include "core/common/exception.h"
#include "middleware/imgui/widgets.h"


namespace ne = ax::NodeEditor;

namespace {

static math::Color GetColorByPinType(uint32_t pinType, uint8_t alpha) {
    static const std::array<math::Color, 7> pinColorByType = {
        math::Color(255, 0, 0),
        math::Color(0, 255, 0),
        math::Color(0, 0, 255),
        math::Color(255, 255, 0),
        math::Color(255, 0, 255),
        math::Color(0, 255, 255),
        math::Color(255, 255, 255),
    };

    auto color = pinColorByType[(pinType & 0xFFFF) % pinColorByType.size()];
    color.alpha = alpha;

    return color;
}

}

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

GraphNode::~GraphNode() {
    for (auto& node : m_inputs) {
        if (node) {
            node->DetachOutput(this);
        }
    }
}

bool GraphNode::IsFull() const noexcept {
    for (const auto& node : m_inputs) {
        if ((!node) || (!node->IsFull())) {
            return false;
        }
    }

    return true;
}

bool GraphNode::AttachInput(uint8_t number, GraphNode* node) {
    if ((number >= m_inputs.size()) || (!node)) {
        return false;
    }

    if (!AttachInputImpl(number, node)) {
        return false;
    }

    m_inputPins[number].isConnected = true;
    m_inputs[number] = node;
    node->AttachOutput(this);
    StateChanged();

    return true;
}

bool GraphNode::DetachInput(uint8_t number) {
    if (number >= m_inputs.size()) {
        return false;
    }

    if (!DetachInputImpl(number)) {
        return false;
    }

    m_inputPins[number].isConnected = false;
    if (auto node = m_inputs[number]) {
        node->DetachOutput(this);
    }
    m_inputs[number] = nullptr;
    StateChanged();

    return true;
}

void GraphNode::Draw(bool isSelected, uint8_t alpha, TextureViewRaw texBackground, float texWidth, float texHeight) {
    const float nodePaddingLeft = 8;
    const float nodePaddingRight = 8;
    const float nodePaddingTop = 4;
    const float nodePaddingBottom = 8;
    const auto iconSize = math::Size(24, 24);
    const auto innerPinColor = math::Color(32, 32, 32, alpha);
    const auto headerColor = math::Color(0, 125, 0, alpha).value;
    const auto headerLineColor = math::Color(255, 255, 255, 96 * alpha / (3 * 255)).value;


    ne::PushStyleVar(ne::StyleVar_NodePadding, ImVec4(nodePaddingLeft, nodePaddingTop, nodePaddingRight, nodePaddingBottom));
    ne::NodeId id(this);
    ne::BeginNode(id);

    // Header
    ImGui::BeginGroup();
    ImGui::TextUnformatted(m_name);
    auto headerMin = ImGui::GetItemRectMin();
    auto headerMax = ImGui::GetItemRectMax();
    auto headerSize = ImGui::GetItemRectSize();
    float dummySize = headerSize.x - (iconSize.w + 8);
    ImGui::Dummy(ImVec2(1.0f, nodePaddingTop));
    ImGui::EndGroup();

    // Input pins
    if (!m_inputPins.empty()) {
        ImGui::BeginGroup();
        for (auto& pin : m_inputPins) {
            ne::BeginPin(ne::PinId(&pin), ne::PinKind::Input);
            gui::NodeIcon(iconSize, gui::IconType::Circle, pin.isConnected, GetColorByPinType(pin.pinType, alpha), innerPinColor);
            ne::EndPin();
        }
        dummySize -= (iconSize.w + 8);
        ImGui::EndGroup();
        ImGui::SameLine();
    }

    ImGui::Dummy(ImVec2(dummySize, 1.0f));

    // Output pin
    if (m_outputPin.pinType != 0) {
        ImGui::SameLine();
        ImGui::BeginGroup();
            ne::BeginPin(ne::PinId(&m_outputPin), ne::PinKind::Output);
            bool filled = ((isSelected && (m_outputs.size() > 1)) || (!isSelected && !m_outputs.empty()));
            gui::NodeIcon(iconSize, gui::IconType::Circle, filled, GetColorByPinType(m_outputPin.pinType, alpha), innerPinColor);
            ne::EndPin();
        ImGui::EndGroup();
    }

    ne::EndNode();

    // Header
    if (ImGui::IsItemVisible()) {
        auto drawList = ne::GetNodeBackgroundDrawList(id);
        const auto texBackgroundID = reinterpret_cast<ImTextureID>(texBackground);
        const auto rounding = ne::GetStyle().NodeRounding;
        const auto roundingCorners = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight;
        const auto uvMin = ImVec2(0.0f, 0.0f);
        const auto uvMax = ImVec2(headerSize.x / texWidth, headerSize.y / texHeight);

        const auto halfBorderWidth = ne::GetStyle().NodeBorderWidth * 0.5f;
        const auto imageTopLeft = ImVec2(
            headerMin.x - nodePaddingLeft + halfBorderWidth,
            headerMin.y - nodePaddingTop + halfBorderWidth);
        const auto imageBottomRight = ImVec2(
            headerMax.x + nodePaddingRight - halfBorderWidth,
            headerMax.y + nodePaddingTop);
        const auto imageBottomLeft = ImVec2(imageTopLeft.x, imageBottomRight.y);

        drawList->AddImageRounded(texBackgroundID, imageTopLeft, imageBottomRight, uvMin, uvMax, headerColor, rounding, roundingCorners);
        drawList->AddLine(imageBottomLeft, imageBottomRight, headerLineColor, 1.0f);
    }
    ne::PopStyleVar(1);
}

void GraphNode::StateChanged() {
    m_isDirty = true;
    for (auto& [node, nodeWeak]: m_outputs) {
        if (!nodeWeak.IsValid()) {
            throw EngineError("GraphNode: output node is removed, can't StateChanged");
        }
        node->StateChanged();
    }
}

void GraphNode::AttachOutput(GraphNode* node) {
    if (m_outputs.find(node) != m_outputs.end()) {
        throw EngineError("GraphNode: double add output node for AttachOutput");
    }
    m_outputPin.isConnected = true;
    m_outputs[node] = Weak(node);
}

void GraphNode::DetachOutput(GraphNode* node) {
    auto it = m_outputs.find(node);
    if (it == m_outputs.end()) {
        throw EngineError("GraphNode: not found output node for delete in DetachOutput");
    }
    m_outputs.erase(it);
    if (m_outputs.empty()) {
        m_outputPin.isConnected = false;
    }
}
