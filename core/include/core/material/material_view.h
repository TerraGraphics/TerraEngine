#pragma once

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/common/pimpl.h"


class MaterialView : Nonmoveable {
public:
    MaterialView() = delete;
    MaterialView(const char* name, PipelineStatePtr pipelineState, ShaderResourceBindingPtr binding);
    MaterialView(const MaterialView& other);
    ~MaterialView();

    void SetVertexShaderVar(const char* name, DeviceRaw value);
    void SetPixelShaderVar(const char* name, DeviceRaw value);
    void SetGeometryShaderVar(const char* name, DeviceRaw value);

    void Bind(ContextPtr& context);

private:
    struct Impl;
    Pimpl<Impl, 24, 8> impl;
};
