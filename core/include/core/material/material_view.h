#pragma once

#include <memory>
#include <string>

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/common/pimpl.h"


class MaterialNew;
class MaterialView : Fixed {
public:
    MaterialView() = delete;
    MaterialView(const std::string& name, const std::shared_ptr<MaterialNew>& material);
    ~MaterialView();

    void UpdatePipeline(PipelineStatePtr pipelineState, ShaderResourceBindingPtr binding);

    void SetVertexShaderVar(const char* name, DeviceRaw value);
    void SetPixelShaderVar(const char* name, DeviceRaw value);
    void SetGeometryShaderVar(const char* name, DeviceRaw value);

    void Update(DevicePtr& device, ContextPtr& context);
    void Bind(ContextPtr& context);

private:
    struct Impl;
    Pimpl<Impl, 64, 8> impl;
};
