#pragma once

#include "core/common/dg.h"
#include "core/common/ctor.h"


namespace Diligent {
    class IPipelineState;
    class IShaderResourceBinding;
}

class Material : Fixed {
public:
    Material() = delete;
    Material(const dg::RefCntAutoPtr<dg::IPipelineState>& pipelineState);

    const dg::Char* GetName() const;
    dg::RefCntAutoPtr<dg::IShaderResourceBinding> CreateShaderResourceBinding();

    void Bind(ContextPtr& context);

private:
    dg::RefCntAutoPtr<dg::IPipelineState> m_pipelineState;
};
