#include "middleware/gscheme/editor/gs_editor.h"

#include <cstdint>
#include <utility>

#include "middleware/gscheme/graph/gs_id.h"
#include "middleware/gscheme/editor/gs_draw.h"
#include "middleware/gscheme/graph/gs_graph.h"
#include "middleware/imgui/imgui_node_editor.h"


GSEditor::GSEditor(const std::string& name, TexturePtr& texBackground)
    : m_name(name)
    , m_config(new ne::Config())
    , m_draw(new gs::Draw(texBackground))
    , m_graph(new gs::Graph(16)) {

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

    m_draw.reset();
    m_graph.reset();
}

void GSEditor::Create() {
    m_config->SettingsFile = "";
    m_context = ne::CreateEditor(m_config);
    m_graph->AddNode("Sum");
    m_graph->AddNode("Constant");
}

void GSEditor::Draw() {
    ne::SetCurrentEditor(m_context);
    ne::PushStyleVar(ne::StyleVar_NodeBorderWidth, 0.f);
    ne::PushStyleVar(ne::StyleVar_HoveredNodeBorderWidth, 2.f);
    ne::PushStyleVar(ne::StyleVar_SelectedNodeBorderWidth, 2.f);
    ne::Begin(m_name.c_str());

    m_graph->DrawGraph(m_draw.get());

    if (ne::BeginCreate()) {
        ne::PinId pinIdFirst, pinIdSecond;
        if (ne::QueryNewLink(&pinIdFirst, &pinIdSecond)) {
            uint32_t srcPinId = static_cast<uint32_t>(pinIdFirst.Get());
            uint32_t dstPinId = static_cast<uint32_t>(pinIdSecond.Get());
            if (gs::IsInputFromPinId(srcPinId)) {
                std::swap(srcPinId, dstPinId);
            }

            if (!m_graph->TestAddLink(srcPinId, dstPinId)) {
                ne::RejectNewItem();
            } else if (ne::AcceptNewItem()) {
                m_graph->AddLink(srcPinId, dstPinId);
            }
        }
        ne::EndCreate();
    }

    ne::End();
    ne::PopStyleVar(3);
}

void GSEditor::DrawProperty() {
    // m_storage->DrawProperty();
}
