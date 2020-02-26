#pragma once

#include <memory>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/common/ctor.h"


class Material;
class GeometryNode;
class MaterialNode : Fixed {
public:
    MaterialNode() = delete;
    MaterialNode(const std::shared_ptr<Material>& material, const std::shared_ptr<GeometryNode>& geometry);
    virtual ~MaterialNode();

    std::shared_ptr<GeometryNode>& GetGeometry() noexcept { return m_geometry; }

    void SetVertexShaderVar(const char* name, DeviceRaw value);
    void SetPixelShaderVar(const char* name, DeviceRaw value);

    virtual void Update(DevicePtr& device, ContextPtr& context) = 0;
    uint32_t Draw(ContextPtr& context, uint32_t firstInstanceIndex = 0);

protected:
    std::shared_ptr<Material> m_material;
    std::shared_ptr<GeometryNode> m_geometry;
    ShaderResourceBindingPtr m_binding;
};
