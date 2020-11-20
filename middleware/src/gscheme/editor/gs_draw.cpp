#include "middleware/gscheme/editor/gs_draw.h"

#include <cstddef>

#include "fmt/fmt.h"
#include "dg/texture.h"
#include "imgui/imgui.h"
#include "imgui/node_editor.h"
#include "dg/graphics_types.h"
#include "middleware/imgui/field.h"
#include "middleware/imgui/label.h"
#include "middleware/gscheme/meta/gs_type_instance.h"
#include "middleware/gscheme/meta/gs_type_interface.h"


namespace gs {

static void DrawPropertyHeader(std::string_view label) {
    gui::LabelStyle labelStyle;
    labelStyle.margin.left = 5;
    gui::Label(label, labelStyle);
    ImGui::NextColumn();
    ImGui::NextColumn();
}

static void DrawPropertyRow(std::string_view propertyName, std::string_view labelText, IPrimitiveType* value, bool withOffset) {
    if (!value->IsEnabledUI()) {
        return;
    }
    gui::LabelStyle labelStyle;
    labelStyle.margin.left = withOffset ? 20 : 5;
    gui::Label(labelText, labelStyle);
    ImGui::NextColumn();

    gui::NumberFieldStyle fieldStyle;
    fieldStyle.margin.left = 5;
    fieldStyle.margin.right = 5;
    fieldStyle.width = gui::NumberFieldStyle::ALL_AVAILABLE;
    fieldStyle.isInteger = value->IsIntegerType();
    fieldStyle.showStepButtons = value->IsEnabledShowStepButtons();
    std::string textValue = value->ToString();

    std::string id = fmt::format("node.property.{}.{}", propertyName, labelText);
    switch (gui::NumberField(id, textValue, fieldStyle)) {
    case gui::NumberFieldAction::StepUp:
        value->Inc();
        break;
    case gui::NumberFieldAction::StepDown:
        value->Dec();
        break;
    case gui::NumberFieldAction::Changed:
        value->FromString(textValue);
        break;
    case gui::NumberFieldAction::None:
        break;
    }
    ImGui::NextColumn();
}

Draw::Draw(TexturePtr& texBackground)
    : m_texBackground(texBackground->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE)) {

    const auto& desc = texBackground->GetDesc();
    m_texBackgroundSize = math::SizeF(static_cast<float>(desc.Width), static_cast<float>(desc.Height));
    m_nodes.resize(64);
}

Draw::~Draw() {

}

void Draw::OnStartDrawGraph() {
    m_alpha = static_cast<uint8_t>(ImGui::GetStyle().Alpha * 255.0f);
    for (auto& node: m_nodes) {
        node.OnStartDrawGraph();
    }
}

void Draw::OnFinishDrawGraph() {
}

void Draw::OnStartDrawNode(uintptr_t id, std::string prettyName) {
    auto nodeIndex = id - 1;
    if (nodeIndex >= m_nodes.size()) {
        m_nodes.resize(nodeIndex + 1);
    }
    m_node = &m_nodes[nodeIndex];
    m_node->OnStartDrawNode(id, prettyName, m_alpha);
}

void Draw::OnFinishDrawNode(bool isValid, std::string_view errorMessage) {
    m_node->OnFinishDrawNode(isValid, errorMessage, m_texBackground.RawPtr(), m_texBackgroundSize);
}

void Draw::OnDrawInputPins(const std::vector<IDraw::Pin>& pins) {
    m_node->OnDrawInputPins(pins);
}

void Draw::OnDrawPinPreview(TypeId typeId, const cpgf::GVariant& value) {
    m_node->OnDrawPinPreview(typeId, value);
}

void Draw::OnDrawOutputPins(const std::vector<IDraw::Pin>& pins) {
    m_node->OnDrawOutputPins(pins);
}

void Draw::OnDrawLink(uintptr_t linkId, uintptr_t srcPinId, uintptr_t dstPinId) {
    ne::Link(ne::LinkId(linkId), ne::PinId(srcPinId), ne::PinId(dstPinId));
}

void Draw::OnStartDrawNodeProperty(const std::string& prettyName) {
    gui::LabelStyle labelStyle;
    labelStyle.margin.left = 5;
    labelStyle.margin.bottom = 10;
    gui::Label(prettyName + ":", labelStyle);

    ImGui::Columns(2, "gs_node_property", true);
}

IDraw::ButtonsState Draw::OnDrawPinProperty(const std::string& prettyName, TypeInstance* typeInstance, bool disabled) {
    if (disabled) {
        gui::StartDisable();
    }
    if (typeInstance->IsPrimitiveType()) {
        DrawPropertyRow("", prettyName, typeInstance->GetValue(0), false);
    } else {
        DrawPropertyHeader(prettyName);
        for (size_t i=0; i!=typeInstance->Count(); ++i) {
            gs::IPrimitiveType* value = typeInstance->GetValue(i);
            DrawPropertyRow(prettyName, value->GetPrettyName(), value, true);
        }
    }
    if (disabled) {
        gui::FinishDisable();
    }

    return IDraw::ButtonsState::None;
}

void Draw::OnFinishDrawNodeProperty() {
    ImGui::Columns(1);
}

}
