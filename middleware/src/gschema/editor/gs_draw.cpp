#include "middleware/gschema/editor/gs_draw.h"

#include <cstddef>

#include "fmt/fmt.h"
#include "dg/texture.h"
#include "imgui/imgui.h"
#include "imgui/node_editor.h"
#include "dg/graphics_types.h"
#include "middleware/imgui/field.h"
#include "middleware/imgui/label.h"
#include "middleware/imgui/combo_box.h"
#include "middleware/gschema/meta/gs_type_instance.h"
#include "middleware/gschema/meta/gs_type_interface.h"


namespace gs {

static void DrawPropertyHeader(std::string_view label) {
    gui::LabelStyle labelStyle;
    labelStyle.margin.left = 5;
    gui::Label(label, labelStyle);
    ImGui::NextColumn();
    ImGui::NextColumn();
}

static void DrawEnumType(std::string_view id, IPrimitiveType* value) {
    gui::ComboBoxStyle style;
    style.margin.left = 5;
    style.margin.right = 5;
    style.width = gui::ComboBoxStyle::ALL_AVAILABLE;

    std::string textValue = value->ToString();
    const auto& items = value->GetItems();
    size_t index = 0;
    for (size_t i=0; i!=items.size(); ++i) {
        if (items[i] == textValue) {
            index = i;
            break;
        }
    }

    if (gui::ComboBox(id, index, items, style)) {
        value->FromString(items[index]);
    }
}

static void DrawArithmeticType(std::string_view id, IPrimitiveType* value) {
    gui::NumberFieldStyle style;
    style.margin.left = 5;
    style.margin.right = 5;
    style.width = gui::NumberFieldStyle::ALL_AVAILABLE;
    style.isInteger = value->IsIntegerType();
    style.showStepButtons = value->IsEnabledShowStepButtons();
    std::string textValue = value->ToString();

    switch (gui::NumberField(id, textValue, style)) {
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
}

static void DrawPropertyRow(std::string_view propertyName, std::string_view labelText, IPrimitiveType* value, bool withOffset) {
    if (!value->IsEnabledUI()) {
        return;
    }
    gui::LabelStyle style;
    style.margin.left = withOffset ? 20 : 5;
    gui::Label(labelText, style);
    ImGui::NextColumn();

    std::string id = fmt::format("node.property.{}.{}", propertyName, labelText);
    if (value->IsEnumType()) {
        DrawEnumType(id, value);
    } else {
        DrawArithmeticType(id, value);
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

void Draw::OnDrawMiniPreview(TypeId typeId, const cpgf::GVariant& value, uint8_t valueVersion) {
    m_node->OnDrawMiniPreview(typeId, value, valueVersion);
}

void Draw::OnDrawOutputPins(const std::vector<IDraw::Pin>& pins) {
    m_node->OnDrawOutputPins(pins);
}

void Draw::OnDrawLink(uintptr_t linkId, uintptr_t srcPinId, uintptr_t dstPinId) {
    ne::Link(ne::LinkId(linkId), ne::PinId(srcPinId), ne::PinId(dstPinId));
}

void Draw::OnDrawFullPreview(const std::string& prettyName) {
    gui::LabelStyle labelStyle;
    labelStyle.margin.left = 5;
    labelStyle.margin.bottom = 10;
    gui::Label(prettyName + ":", labelStyle);
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
