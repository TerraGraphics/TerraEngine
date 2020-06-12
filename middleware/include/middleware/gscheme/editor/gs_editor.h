#pragma once

#include <memory>
#include <string>
#include <cstdint>

#include "dg/dg.h"
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
class TypeStorage;
class Editor : Fixed {
public:
    Editor() = delete;
    Editor(const std::shared_ptr<TypeStorage>& typeStorage, const std::string& name, TexturePtr& texBackground);
    ~Editor();

    void Create();
    void DrawGraph();
    void DrawNodeProperty();

private:
    void DrawNewNodeMenu(float x, float y);

private:
    std::string m_name;
    ax::NodeEditor::Config* m_config = nullptr;
    ax::NodeEditor::EditorContext* m_context = nullptr;
    std::unique_ptr<Draw> m_draw;
    std::unique_ptr<Graph> m_graph;
    std::shared_ptr<TypeStorage> m_typeStorage;
    uint16_t m_selectedNodeId = 0;
};

}