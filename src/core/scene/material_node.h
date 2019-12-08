#pragma once

#include <memory>

#include "core/common/dg.h"
#include "core/common/ctor.h"


class Material;
class GeometryNode;
namespace Diligent {
    class IDeviceObject;
    class IShaderResourceBinding;
}
class MaterialNode : Fixed {
public:
    MaterialNode() = delete;
    MaterialNode(const std::shared_ptr<Material>& material, const std::shared_ptr<GeometryNode>& geometry);

    void SetVertexShaderVar(const dg::Char* name, dg::IDeviceObject* value);
    void SetPixelShaderVar(const dg::Char* name, dg::IDeviceObject* value);

    void Bind(ContextPtr& context);
    uint32_t Draw(ContextPtr& context);
private:
    std::shared_ptr<Material> m_material;
    std::shared_ptr<GeometryNode> m_geometry;
    dg::RefCntAutoPtr<dg::IShaderResourceBinding> m_binding;
};
