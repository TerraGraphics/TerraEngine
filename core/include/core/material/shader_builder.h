#pragma once

#include <string>
#include <cstddef>
#include <unordered_map>

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/material/microshader_loader.h"
#include "core/material/material_builder_desc.h"


class ShaderBuilder : Fixed {
public:
    struct Shaders {
        ShaderPtr vs;
        ShaderPtr ps;
        ShaderPtr gs;
    };

private:
    struct CacheKey {
        dg::SHADER_TYPE shaderType;
        std::string source;

        // hash function
        size_t operator()(const CacheKey& value) const;
        bool operator==(const CacheKey& other) const;
    };

public:
    ShaderBuilder() = delete;
    ShaderBuilder(const DevicePtr& device, const EngineFactoryPtr& engineFactory);
    ~ShaderBuilder() = default;

    void Create(const MaterialBuilderDesc& desc);
    Shaders Build(const MicroshaderLoader::Source& source);

private:
    ShaderPtr BuildSource(const CacheKey& shaderSrc, const std::string& name);

private:
    DevicePtr m_device;
    EngineFactoryPtr m_engineFactory;

    MaterialBuilderDesc m_desc;
    ShaderSourceInputStreamFactoryPtr m_shaderSourceFactory;

    std::unordered_map<CacheKey, ShaderPtr, CacheKey> m_cache;
};
