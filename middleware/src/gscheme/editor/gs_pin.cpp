#include "middleware/gscheme/editor/gs_pin.h"

#include "core/math/types.h"
#include "middleware/imgui/widgets.h"
#include "middleware/imgui/imgui_node_editor.h"
#include "middleware/gscheme/reflection/gs_type.h"


static const auto ICON_SIZE = math::Size(24, 24);

GSInputPin::GSInputPin(uintptr_t id, rttr::variant& instance, const rttr::property& property)
    : m_id(id)
    , m_type(CreateGSType(instance, property)) {

}

GSInputPin::~GSInputPin() {

}

void GSInputPin::SetValue(const rttr::variant& value) {
    m_type->SetValue(value);
}

void GSInputPin::Draw(uint8_t alpha) const {
    if (!m_type->IsEmbedded()) {
        const auto pinColor = math::Color(0, 255, 0, alpha);
        const auto innerPinColor = math::Color(32, 32, 32, alpha);

        ne::BeginPin(ne::PinId(m_id), ne::PinKind::Input);
        gui::NodeIcon(ICON_SIZE, gui::IconType::Circle, m_isConnected, pinColor, innerPinColor);
        ne::EndPin();

        gui::SameLine();
        gui::BeginGroup();
        gui::Dummy(math::SizeF(0, 1.f));
        gui::Text(m_type->GetName());
        gui::EndGroup();
    }
}

void GSInputPin::DrawEditGui() {
    m_type->DrawEditGui();
}

GSOutputPin::GSOutputPin(uintptr_t id, rttr::variant& instance, const rttr::property& property)
    : m_id(id)
    , m_type(CreateGSType(instance, property)) {

}

GSOutputPin::~GSOutputPin() {

}

rttr::variant GSOutputPin::GetValue() const {
    return m_type->GetValue();
}

void GSOutputPin::Draw(uint8_t alpha) const {
    const auto pinColor = math::Color(0, 255, 0, alpha);
    const auto innerPinColor = math::Color(32, 32, 32, alpha);

    gui::BeginGroup();
    gui::Dummy(math::SizeF(0, 1.f));
    gui::Text(m_type->GetName());
    gui::EndGroup();
    gui::SameLine();

    ne::BeginPin(ne::PinId(m_id), ne::PinKind::Output);
    gui::NodeIcon(ICON_SIZE, gui::IconType::Circle, m_isConnected, pinColor, innerPinColor);
    ne::EndPin();
}
