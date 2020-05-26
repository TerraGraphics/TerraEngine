#include "middleware/gscheme/editor/gs_pin.h"

#include "core/math/types.h"
#include "middleware/imgui/widgets.h"
#include "middleware/imgui/imgui_node_editor.h"


GSInputPin::GSInputPin(uintptr_t id, const std::string& name)
    : m_id(id)
    , m_name(name) {

}

GSInputPin::~GSInputPin() {

}

void GSInputPin::Draw(uint8_t alpha) const {
    const auto iconSize = math::Size(24, 24);
    const auto pinColor = math::Color(0, 255, 0, alpha);
    const auto innerPinColor = math::Color(32, 32, 32, alpha);

    ne::BeginPin(ne::PinId(m_id), ne::PinKind::Input);
    gui::NodeIcon(iconSize, gui::IconType::Circle, m_isConnected, pinColor, innerPinColor);
    ne::EndPin();
}
