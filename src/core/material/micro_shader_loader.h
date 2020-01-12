#pragma once

#include <ucl++.h>
#include "core/common/ctor.h"
#include "core/material/material_builder_desc.h"


class MicroShaderLoader : Fixed {
public:
    struct Source {
        std::string name;
        std::string vs;
        std::string ps;
        std::string gs;
    };

private:
    struct InputType {
        std::string type;
        std::string semantic;
    };
    struct ShaderData {
        void Append(const ShaderData& other);
        std::string GenParametersToStr(const MaterialBuilderDesc& desc);

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
    MicroShaderLoader() = default;
    ~MicroShaderLoader() = default;

    void Load(const MaterialBuilderDesc& desc);
    uint64_t GetMask(const std::string& name) const;
    Source GetSources(uint64_t mask) const;

private:
    void ParseMicroshader(const ucl::Ucl& section);
    void ParseShader(const ucl::Ucl& section, const std::string& sectionName, ShaderData& shader);
    void ParseParameters(const ucl::Ucl& section, const std::string& sectionName, ShaderData& shader);
    void ParseInputs(const ucl::Ucl& section, const std::string& sectionName, ShaderData& shader);

private:
    MaterialBuilderDesc m_desc;

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
