#include "core/material/shader_builder.h"

#include <utility>
#include <filesystem>
#include <fmt/format.h>

#include <DiligentCore/Primitives/interface/DataBlob.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/Shader.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/EngineFactory.h>

#include "core/dg/errors.h"
#include "core/common/exception.h"
#include "core/dg/render_device.h"
#include "core/common/hash_combine.h"
#include "core/dg/graphics_accessories.h"


std::size_t ShaderBuilder::CacheKey::operator()(const ShaderBuilder::CacheKey& value) const {
    std::size_t h = 0;
    hash_combine(h, value.shaderType);
    hash_combine(h, value.source);

    return h;
}

bool ShaderBuilder::CacheKey::operator==(const ShaderBuilder::CacheKey& other) const {
    return ((shaderType == other.shaderType) && (source == other.source));
}

ShaderBuilder::ShaderBuilder(const DevicePtr& device, const EngineFactoryPtr& engineFactory)
    : m_device(device)
    , m_engineFactory(engineFactory) {

}

void ShaderBuilder::Create(const MaterialBuilderDesc& desc) {
    m_desc = desc;
    m_engineFactory->CreateDefaultShaderSourceStreamFactory(m_desc.shadersDir.c_str(), &m_shaderSourceFactory);
}

ShaderBuilder::Shaders ShaderBuilder::Build(const MicroshaderLoader::Source& source) {
    Shaders result;
    result.vs = BuildSource(CacheKey{dg::SHADER_TYPE_VERTEX, source.vs}, source.name);
    result.ps = BuildSource(CacheKey{dg::SHADER_TYPE_PIXEL, source.ps}, source.name);
    result.gs = BuildSource(CacheKey{dg::SHADER_TYPE_GEOMETRY, source.gs}, source.name);

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

ShaderPtr ShaderBuilder::BuildSource(const CacheKey& shaderSrc, const std::string& name) {
    ShaderPtr shader;

    if (shaderSrc.source.empty()) {
        return shader;
    }

    const auto it = m_cache.find(shaderSrc);
    if (it != m_cache.cend()) {
        return it->second;
    }

    std::string fullName;
    switch (shaderSrc.shaderType) {
        case dg::SHADER_TYPE_VERTEX:
            fullName = "vs.";
            break;
        case dg::SHADER_TYPE_PIXEL:
            fullName = "ps.";
            break;
        case dg::SHADER_TYPE_GEOMETRY:
            fullName = "gs.";
            break;
        default:
            fullName = "unknown.";
            break;
    }
    fullName += name;

    dg::IDataBlob* compilerOutput = nullptr;

    dg::ShaderCreateInfo shaderCI;
    shaderCI.pShaderSourceStreamFactory = m_shaderSourceFactory;
    shaderCI.Source = shaderSrc.source.c_str();
    shaderCI.EntryPoint = "main";
    shaderCI.UseCombinedTextureSamplers = true;
    shaderCI.CombinedSamplerSuffix = m_desc.samplerSuffix.c_str();
    shaderCI.Desc.ShaderType = shaderSrc.shaderType;
    shaderCI.Desc.Name = fullName.c_str();
    shaderCI.SourceLanguage = dg::SHADER_SOURCE_LANGUAGE_HLSL;
    shaderCI.ppCompilerOutput = &compilerOutput;

    try {
        m_device->CreateShader(shaderCI, &shader);
    } catch (const std::exception& e) {
        ProcessBuildError(shaderSrc.source, name, shaderSrc.shaderType, compilerOutput, e.what());
    }

    if (!shader) {
        ProcessBuildError(shaderSrc.source, name, shaderSrc.shaderType, compilerOutput, nullptr);
    }
    if (compilerOutput) {
        compilerOutput->Release();
    }

    m_cache[shaderSrc] = shader;

    return shader;
}
