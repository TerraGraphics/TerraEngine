#pragma once

#include <memory>
#include <DiligentCore/Common/interface/BasicMath.h>

#include "core/common/dg.h"
#include "core/common/ctor.h"

class Material;
class GeometryNode;
namespace Diligent {
    #include "structures.fxh"
    class IBuffer;
    class IDeviceObject;
    class IShaderResourceBinding;
}
class MaterialNode : Fixed {
public:
    MaterialNode() = delete;
    MaterialNode(const std::shared_ptr<Material>& material, const std::shared_ptr<GeometryNode>& geometry);

    std::shared_ptr<GeometryNode> GetGeometry() noexcept { return m_geometry; }

    void SetVertexShaderVar(const dg::Char* name, dg::IDeviceObject* value);
    void SetPixelShaderVar(const dg::Char* name, dg::IDeviceObject* value);

    void Bind(DevicePtr& device, ContextPtr& context);
    uint32_t Draw(ContextPtr& context, uint32_t firstInstanceIndex = 0);

protected:
    virtual void BindVars(DevicePtr& device, ContextPtr& context) {};

private:
    std::shared_ptr<Material> m_material;
    std::shared_ptr<GeometryNode> m_geometry;
    dg::RefCntAutoPtr<dg::IShaderResourceBinding> m_binding;
};

class MaterialClrNode : public MaterialNode {
public:
    MaterialClrNode() = delete;
    MaterialClrNode(const std::shared_ptr<Material>& material, const std::shared_ptr<GeometryNode>& geometry);

    dg::ShaderMaterial& Params() noexcept { m_isDirty = true; return m_materialParams; };

    void BindVars(DevicePtr& device, ContextPtr& context) override;

private:
    bool m_isDirty = true;
    dg::ShaderMaterial m_materialParams;
    dg::ShaderMaterial m_materialParamsFinish;
    dg::RefCntAutoPtr<dg::IBuffer> m_materialCB;
};
