#pragma once

#include <memory>
#include <string>

#include "core/dg/dg.h"
#include "core/common/ctor.h"


namespace ax {
    namespace NodeEditor {
        struct Config;
        struct EditorContext;
    }
}

namespace gs {
    class Draw;
    class Graph;
}

class GSEditor : Fixed {
public:
    GSEditor() = delete;
    GSEditor(const std::string& name, TexturePtr& texBackground);
    ~GSEditor();

    void Create();
    void Draw();
    void DrawProperty();

private:
    std::string m_name;
    ax::NodeEditor::Config* m_config = nullptr;
    ax::NodeEditor::EditorContext* m_context = nullptr;
    std::unique_ptr<gs::Draw> m_draw;
    std::unique_ptr<gs::Graph> m_graph;
};
