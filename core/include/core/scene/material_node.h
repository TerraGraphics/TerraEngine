#pragma once

#include <memory>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/common/ctor.h"


class Material;
class MaterialNode : Fixed {
public:
    MaterialNode() = delete;
    MaterialNode(const std::shared_ptr<Material>& material);
    virtual ~MaterialNode();

    void SetVertexShaderVar(const char* name, DeviceRaw value);
    void SetPixelShaderVar(const char* name, DeviceRaw value);

    virtual void Update(DevicePtr& device, ContextPtr& context) = 0;
    void Bind(ContextPtr& context);

protected:
    std::shared_ptr<Material> m_material;
    ShaderResourceBindingPtr m_binding;
};
