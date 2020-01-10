#pragma once

#include <ucl++.h>
#include <functional>
#include <filesystem>
#include "core/common/ctor.h"


class MicroShaderLoader : Fixed {
public:
    struct Source {
        std::string name;
        std::string vs;
        std::string ps;
        std::string gs;
    };

    using CBufferGenerator = std::function<std::string (const std::string&)>;

private:
    struct InputType {
        std::string type;
        std::string semantic;
    };
    struct ShaderData {
        void Append(const ShaderData& other);
        std::string GenParametersToStr(const std::string& samplerSuffix, const CBufferGenerator& cBufferGenerator);

        std::set<std::string> includes;
        std::set<std::string> textures2D;
        std::set<std::string> cbuffers;
        std::map<std::string, InputType> inputs;
        std::string source;
    };

    struct Microshader {
        bool isEmpty = true;
        std::string name;
        std::string group;
        uint32_t groupID = 0;

        ShaderData vs;
        ShaderData ps;
        ShaderData gs;
    };

public:
    MicroShaderLoader();
    ~MicroShaderLoader() = default;

    void Load(const std::filesystem::path& dirPath, const std::string& extension);
    void SetSamplerSuffix(const std::string& value);
    void SetCBufferGenerator(const CBufferGenerator& value);
    uint64_t GetMask(const std::string& name) const;
    Source GetSources(uint64_t mask) const;

private:
    void ParseMicroshader(const ucl::Ucl& section);
    void ParseShader(const ucl::Ucl& section, const std::string& sectionName, ShaderData& shader);
    void ParseParameters(const ucl::Ucl& section, const std::string& sectionName, ShaderData& shader);
    void ParseInputs(const ucl::Ucl& section, const std::string& sectionName, ShaderData& shader);

private:
    std::string m_samplerSuffix = "Sampler";
    CBufferGenerator m_cBufferGenerator;

    Microshader m_root;
    // namedMicroShaderID => namedMicroShader
    std::vector<Microshader> m_namedMicroShaders;
    // namedMicroShaderName => namedMicroShaderID
    std::map<std::string, uint32_t> m_namedMicroShaderIDs;
    // groupID => defaultMicroShader
    std::vector<Microshader> m_defaultMicroShaders;
    // groupName => groupID
    std::map<std::string, uint32_t> m_groupIDs;
};
