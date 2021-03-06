#pragma once

#include <memory>
#include <string>
#include <cstdint>

#include "dg/dg.h"
#include "core/common/ctor.h"
#include "core/common/pimpl.h"
#include "core/material/material_view.h"


class MaterialBuilder;
class Material : Fixed {
protected:
    Material() = delete;
    Material(const std::string& name, const std::shared_ptr<MaterialBuilder>& builder);
    virtual ~Material();

public:
    MaterialView GetView(uint8_t frameNum, uint16_t targetsId, uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance);

protected:
    const std::string& GetName() const noexcept;
    std::shared_ptr<MaterialBuilder>& GetBuilder() noexcept;

    void ResetCache();
    uint64_t GetShadersMask() const noexcept;
    void SetShadersMask(uint64_t mask);
    void DepthEnable(bool value) noexcept;
    void CullMode(dg::CULL_MODE value) noexcept;
    void Topology(dg::PRIMITIVE_TOPOLOGY value) noexcept;
    void AddShaderVar(uint16_t varId);

    void SetVertexShaderVar(const char* name, DeviceRaw value);
    void SetPixelShaderVar(const char* name, DeviceRaw value);
    void SetGeometryShaderVar(const char* name, DeviceRaw value);

    virtual void OnNewFrame() = 0;
    virtual uint64_t OnBeforeCreateView(uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance) = 0;
    virtual void OnAfterCreateView(MaterialView& view) = 0;

private:
    struct Impl;
    Pimpl<Impl, 344, 8> impl;
};
