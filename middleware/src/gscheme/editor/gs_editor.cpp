#include "middleware/gscheme/editor/gs_editor.h"

#include "middleware/imgui/imgui_node_editor.h"
#include "middleware/gscheme/editor/gs_storage.h"


GSEditor::GSEditor(const std::string& name, TexturePtr& texBackground)
    : m_name(name)
    , m_config(new ne::Config())
    , m_storage(std::make_shared<GSStorage>(texBackground)) {

}

GSEditor::~GSEditor() {
    if (m_context) {
        ne::DestroyEditor(m_context);
        m_context = nullptr;
    }

    if (m_config) {
        delete m_config;
        m_config = nullptr;
    }

    m_storage.reset();
}

void GSEditor::Create() {
    m_config->SettingsFile = "";
    m_context = ne::CreateEditor(m_config);
    m_storage->Create();
}

void GSEditor::Draw() {
    ne::SetCurrentEditor(m_context);
    ne::PushStyleVar(ne::StyleVar_NodeBorderWidth, 0.f);
    ne::PushStyleVar(ne::StyleVar_HoveredNodeBorderWidth, 2.f);
    ne::PushStyleVar(ne::StyleVar_SelectedNodeBorderWidth, 2.f);
    ne::Begin(m_name.c_str());

    m_storage->Draw();

    ne::End();
    ne::PopStyleVar(3);
}

std::function<void ()> GSEditor::DrawProperty() {
    return [storage = m_storage]() {
        storage->DrawProperty();
    };
}
