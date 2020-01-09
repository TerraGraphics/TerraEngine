#include "core/material/shader_builder.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/EngineFactory.h>
#include <DiligentCore/Graphics/GraphicsAccessories/interface/GraphicsAccessories.h>

#include "core/common/exception.h"


ShaderBuilder::ShaderBuilder(const DevicePtr& device, const EngineFactoryPtr& engineFactory)
    : m_device(device) {
    engineFactory->CreateDefaultShaderSourceStreamFactory("materials", &m_shaderSourceFactory);
}

ShaderBuilder::Shaders ShaderBuilder::Build(const MicroShaderLoader::Source& source) {
    auto vs = BuildSource(source.vs, source.name, dg::SHADER_TYPE_VERTEX);
    auto ps = BuildSource(source.ps, source.name, dg::SHADER_TYPE_PIXEL);
    auto gs = BuildSource(source.gs, source.name, dg::SHADER_TYPE_GEOMETRY);

    return Shaders {vs, ps, gs};
}

dg::RefCntAutoPtr<dg::IShader> ShaderBuilder::BuildSource(const std::string& text, const std::string& name, dg::SHADER_TYPE shaderType) {
    dg::RefCntAutoPtr<dg::IShader> shader;

    if (text.empty()) {
        return shader;
    }

    dg::RefCntAutoPtr<dg::IDataBlob> compilerOutput;

    dg::ShaderCreateInfo ShaderCI;
    ShaderCI.pShaderSourceStreamFactory = m_shaderSourceFactory;
    ShaderCI.Source = text.c_str();
    ShaderCI.EntryPoint = "main";
    ShaderCI.UseCombinedTextureSamplers = true;
    ShaderCI.CombinedSamplerSuffix = "Sampler";
    ShaderCI.Desc.ShaderType = shaderType;
    ShaderCI.Desc.Name = "vs::standart";
    ShaderCI.SourceLanguage = dg::SHADER_SOURCE_LANGUAGE_HLSL;
    ShaderCI.ppCompilerOutput = &compilerOutput;

    try {
        m_device->CreateShader(ShaderCI, &shader);
    } catch (const std::exception& e) {
        LOG_ERROR_MESSAGE("Failed to build ", dg::GetShaderTypeLiteralName(shaderType), " shader ", name, ":\n<<<<<<<<<\n", text, "\n<<<<<<<<<\nError: ", e.what());
        throw EngineError("failed to build {} shader {}, error: {}", dg::GetShaderTypeLiteralName(shaderType), name, e.what());
    }

    if (!shader) {
        // SetDebugMessageCallback
        LOG_ERROR_MESSAGE("Failed to build ", dg::GetShaderTypeLiteralName(shaderType), " shader ", name, ":\n<<<<<<<<<\n", text, "\n<<<<<<<<<\nError: ", reinterpret_cast<char*>(compilerOutput->GetDataPtr()));
        throw EngineError("failed to build {} shader {}", dg::GetShaderTypeLiteralName(shaderType), name);
    }

    return shader;
}
