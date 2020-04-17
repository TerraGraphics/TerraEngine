#include "middleware/graph_editor/node_preview.h"

#include <imgui.h>

#include "core/dg/texture.h"
#include "middleware/imgui/widgets.h"
#include "middleware/imgui/imgui_math.h"
#include "middleware/generator/texture/noise_rasterization.h"


NodePreview::NodePreview(bool isOpenGL)
    : m_isOpenGL(isOpenGL) {

}

void NodePreview::SetNode(NoiseRasterization2D* node) {
    m_node = node;
}

void NodePreview::ResetNode() {
    m_node.Release();
}

void NodePreview::Draw() {
    if (!m_node || !m_node->IsFull()) {
        return;
    }

    TextureViewPtr texView;
    texView = m_node->Get()->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    Image(texView.RawPtr(), ToSize(ImGui::GetContentRegionAvail()), m_isOpenGL);
}
