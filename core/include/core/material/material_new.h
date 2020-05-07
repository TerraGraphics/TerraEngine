#pragma once

#include <memory>
#include <string>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/common/pimpl.h"


namespace Diligent {
    struct SamplerDesc;
}
class MaterialBuilder;
class MaterialNew : Fixed {
public:
    MaterialNew() = delete;
    MaterialNew(const std::shared_ptr<MaterialBuilder>& builder);
    ~MaterialNew();

protected:
    dg::SamplerDesc& GetTextureDesc(uint8_t id);

    void SetShaders(uint64_t mask, uint32_t vDeclIdPerVertex, uint32_t vDeclIdPerInstance);
    void DepthEnable(bool value) noexcept;
    void CullMode(dg::CULL_MODE value) noexcept;
    void Topology(dg::PRIMITIVE_TOPOLOGY value) noexcept;
    uint8_t AddVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type);
    uint8_t AddTextureVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type);

    void Build(const char* name = nullptr);

private:
    struct Impl;
    Pimpl<Impl, 2632, 8> impl;
};
