#include "core/scene/scene.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <DiligentCore/Graphics/GraphicsTools/interface/GraphicsUtilities.h>

#include "core/scene/material_node.h"


Scene::Scene(DevicePtr device, ContextPtr context)
    : m_device(device)
    , m_context(context) {

}

void Scene::Update() {
    m_nodeListForDraw.clear();
    UpdateGraph(m_device, m_context, m_nodeListForDraw);
}

uint32_t Scene::Draw() {
    uint32_t primitiveCount = 0;
    for (const auto& node :m_nodeListForDraw) {
        auto materialNode = node->GetMaterialNode();
        auto cb = node->GetTransformCB();
        materialNode->SetVertexShaderVar("Transform", cb);
        materialNode->Bind(m_device, m_context);
        primitiveCount += materialNode->Draw(m_context);
    }

    return primitiveCount;
}
