#pragma once

#include <string>

#include "core/dg/dg.h"
#include "core/common/ctor.h"


namespace ax {
    namespace NodeEditor {
        struct Config;
        struct EditorContext;
    }
}

class GSEditor : Fixed {
public:
    GSEditor() = delete;
    GSEditor(const std::string& name, TexturePtr& texBackground);
    ~GSEditor();

    void Draw();

private:
    std::string m_name;
    ax::NodeEditor::Config* m_config = nullptr;
    ax::NodeEditor::EditorContext* m_context = nullptr;
};
