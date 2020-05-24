#pragma once

#include <memory>
#include <string>
#include <vector>

#include "core/dg/dg.h"
#include "core/common/ctor.h"


namespace ax {
    namespace NodeEditor {
        struct Config;
        struct EditorContext;
    }
}

class GSNodeType;
class GSEditor : Fixed {
public:
    GSEditor() = delete;
    GSEditor(const std::string& name, TexturePtr& texBackground);
    ~GSEditor();

    void Create();
    void Draw();

private:
    std::string m_name;
    ax::NodeEditor::Config* m_config = nullptr;
    ax::NodeEditor::EditorContext* m_context = nullptr;
    std::vector<std::shared_ptr<GSNodeType>> m_nodeTypes;
};
