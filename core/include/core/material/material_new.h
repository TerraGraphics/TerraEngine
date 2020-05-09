#pragma once

#include <memory>
#include <string>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/common/pimpl.h"
#include "core/material/material_view.h"


namespace Diligent {
    struct SamplerDesc;
}
class MaterialBuilder;
class MaterialNew : Fixed {
protected:
    MaterialNew() = delete;
    MaterialNew(const std::string& name, const std::shared_ptr<MaterialBuilder>& builder);
    virtual ~MaterialNew();

public:
    MaterialView GetView(uint8_t frameNum, uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance);

protected:
    dg::SamplerDesc& GetTextureDesc(uint8_t id);

    const std::string& GetName() const noexcept;
    uint64_t GetShadersMask() const noexcept;
    void SetShadersMask(uint64_t mask);
    void DepthEnable(bool value) noexcept;
    void CullMode(dg::CULL_MODE value) noexcept;
    void Topology(dg::PRIMITIVE_TOPOLOGY value) noexcept;
    uint8_t AddVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type);
    uint8_t AddTextureVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type);

    void SetVertexShaderVar(const char* name, DeviceRaw value);
    void SetPixelShaderVar(const char* name, DeviceRaw value);
    void SetGeometryShaderVar(const char* name, DeviceRaw value);

    virtual void OnNewFrame() = 0;
    virtual void OnNewView(MaterialView& view) = 0;

private:
    struct Impl;
    Pimpl<Impl, 2672, 8> impl;
};
