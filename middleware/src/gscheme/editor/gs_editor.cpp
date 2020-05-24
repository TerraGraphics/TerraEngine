#include "middleware/gscheme/editor/gs_editor.h"

#include "middleware/gscheme/rttr/type.h"
#include "middleware/imgui/imgui_node_editor.h"
#include "middleware/gscheme/editor/gs_node_type.h"
#include "middleware/gscheme/reflection/gs_metadata.h"


GSEditor::GSEditor(const std::string& name, TexturePtr& /* texBackground */)
    : m_name(name)
    , m_config(new ne::Config()) {
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
}

void GSEditor::Create() {
    m_config->SettingsFile = "";
    m_context = ne::CreateEditor(m_config);

    for(const auto& t : rttr::type::get_types()) {
        if (t.get_metadata(GSMetaTypes::GS_CLASS).is_valid()) {
            m_nodeTypes.push_back(std::make_shared<GSNodeType>(t));
        }
    }
}

void GSEditor::Draw() {
    ne::SetCurrentEditor(m_context);
    ne::PushStyleVar(ne::StyleVar_NodeBorderWidth, 0.f);
    ne::PushStyleVar(ne::StyleVar_HoveredNodeBorderWidth, 2.f);
    ne::PushStyleVar(ne::StyleVar_SelectedNodeBorderWidth, 2.f);
    ne::Begin(m_name.c_str());

    ne::End();
    ne::PopStyleVar(3);
}
