#pragma once

#include <DiligentCore/Graphics/GraphicsEngine/interface/Shader.h>

#include "core/common/dg.h"
#include "core/material/micro_shader_loader.h"


class ShaderBuilder : Fixed {
public:
    struct Shaders {
        dg::RefCntAutoPtr<dg::IShader> vs;
        dg::RefCntAutoPtr<dg::IShader> ps;
        dg::RefCntAutoPtr<dg::IShader> gs;
    };

public:
    ShaderBuilder() = delete;
    ShaderBuilder(const DevicePtr& device, const EngineFactoryPtr& engineFactory);
    ~ShaderBuilder() = default;

    void Create(const MaterialBuilderDesc& desc);
    Shaders Build(const MicroShaderLoader::Source& source);

private:
    dg::RefCntAutoPtr<dg::IShader> BuildSource(const std::string& text, const std::string& name, dg::SHADER_TYPE shaderType);

private:
    DevicePtr m_device;
    EngineFactoryPtr m_engineFactory;

    MaterialBuilderDesc m_desc;
    dg::RefCntAutoPtr<dg::IShaderSourceInputStreamFactory> m_shaderSourceFactory;

    std::vector<dg::RefCntAutoPtr<dg::IShader>> m_cache;
};
