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
    Shaders result;
    result.vs = BuildSource(source.vs, source.name, dg::SHADER_TYPE_VERTEX);
    result.ps = BuildSource(source.ps, source.name, dg::SHADER_TYPE_PIXEL);
    result.gs = BuildSource(source.gs, source.name, dg::SHADER_TYPE_GEOMETRY);

    m_cache.push_back(result.vs);
    m_cache.push_back(result.ps);
    m_cache.push_back(result.gs);
    return  result;
}

static void ProcessBuildError(const std::string& text, const std::string& name, dg::SHADER_TYPE shaderType, dg::IDataBlob* compilerOutput, const char* exceptionMsg) {
    const char* typeStr = dg::GetShaderTypeLiteralName(shaderType);

    std::string errorMessage;
    std::string parsedShaderCode;
    if (compilerOutput != nullptr) {
        char* rawPtr = reinterpret_cast<char*>(compilerOutput->GetDataPtr());
        auto firstMsg = std::string(rawPtr);
        errorMessage = "\nError: " + firstMsg;
        parsedShaderCode = "\nParsed shader code:\n<<<<<<<<<\n" + std::string(rawPtr + firstMsg.size() + 1) + "\n<<<<<<<<<";
        compilerOutput->Release();
    }

    std::string shaderCode = "\nShader code:\n<<<<<<<<<\n" + text + "\n<<<<<<<<<";

    std::string exceptionStr;
    if (exceptionMsg) {
        exceptionStr = "\nexception: " + std::string(exceptionMsg);
    }

    auto msg = fmt::format("Failed to build {} shader '{}', {}{}{}{}", typeStr, name, shaderCode, parsedShaderCode, errorMessage, exceptionStr);
    LOG_ERROR_MESSAGE(msg.c_str());

    throw EngineError("failed to build {} shader {}", typeStr, name);
}

dg::RefCntAutoPtr<dg::IShader> ShaderBuilder::BuildSource(const std::string& text, const std::string& name, dg::SHADER_TYPE shaderType) {
    dg::RefCntAutoPtr<dg::IShader> shader;

    if (text.empty()) {
        return shader;
    }

    dg::IDataBlob* compilerOutput = nullptr;

    dg::ShaderCreateInfo shaderCI;
    shaderCI.pShaderSourceStreamFactory = m_shaderSourceFactory;
    shaderCI.Source = text.c_str();
    shaderCI.EntryPoint = "main";
    shaderCI.UseCombinedTextureSamplers = true;
    shaderCI.CombinedSamplerSuffix = "Sampler";
    shaderCI.Desc.ShaderType = shaderType;
    shaderCI.Desc.Name = "vs::standart";
    shaderCI.SourceLanguage = dg::SHADER_SOURCE_LANGUAGE_HLSL;
    shaderCI.ppCompilerOutput = &compilerOutput;

    try {
        m_device->CreateShader(shaderCI, &shader);
    } catch (const std::exception& e) {
        ProcessBuildError(text, name, shaderType, compilerOutput, e.what());
    }

    if (!shader) {
        ProcessBuildError(text, name, shaderType, compilerOutput, nullptr);
    }
    if (compilerOutput) {
        compilerOutput->Release();
    }

    return shader;
}
