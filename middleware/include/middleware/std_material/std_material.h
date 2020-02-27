#pragma once

#include <memory>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/math/basic.h"
#include "core/scene/material_node.h"


namespace Diligent {
    #include "structures.fxh"
}

class Material;
class GeometryNode;
class StdMaterial : public MaterialNode {
public:
    StdMaterial() = delete;
    StdMaterial(const std::shared_ptr<Material>& material, const std::shared_ptr<GeometryNode>& geometry);
    ~StdMaterial() override;

    void SetBaseColor(const math::Color4f& value);
    void SetBaseColor(const dg::float4& value);
    void SetBaseColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void SetBaseTexture(TextureViewPtr& texture);
    void SetAlphaThreshold(float value);

    void Update(DevicePtr& device, ContextPtr& context) override;

private:
    bool m_isMaterialDataExists = true;
    BufferPtr m_materialCB;
    dg::ShaderMaterial m_materialData;
};
