#include "core/scene/scene.h"

#include <vector>
#include <memory>

#include "core/dg/context.h" // IWYU pragma: keep
#include "core/scene/geometry.h"
#include "core/dg/graphics_utilities.h" // IWYU pragma: keep
#include "core/material/material_view.h"


TransformUpdateDesc& Scene::Update(uint16_t targetsId, uint16_t vDeclIdPerInstance, uint32_t findId) {
    m_updateDesc.nodeList.clear();
    m_updateDesc.targetsId = targetsId;
    m_updateDesc.vDeclIdPerInstance = vDeclIdPerInstance;
    m_updateDesc.findId = findId;
    m_updateDesc.findResult.reset();
    UpdateGraph(m_updateDesc);

    return m_updateDesc;
}

uint32_t Scene::Draw(ContextPtr& context) {
    uint32_t primitiveCount = 0;

    uint32_t ind = 0;
    for (auto& node: m_updateDesc.nodeList) {
        node.geometry->Bind(context);
        node.materialView.Bind(context);
        primitiveCount += node.geometry->Draw(context, ind);
        ++ind;
    }

    return primitiveCount;
}
