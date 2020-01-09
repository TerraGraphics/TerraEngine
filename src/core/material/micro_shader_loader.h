#pragma once

#include <ucl++.h>
#include <filesystem>
#include "core/common/ctor.h"


class MicroShaderLoader : Fixed {
    struct ShaderData {
        std::set<std::string> textures2D;
        std::set<std::string> cbuffers;
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
    struct Source {
        std::string vs;
        std::string ps;
        std::string gs;
    };

public:
    MicroShaderLoader() = default;
    ~MicroShaderLoader() = default;

    void Load(const std::filesystem::path& dirPath, const std::string& extension);

private:
    void ParseMicroshader(const ucl::Ucl& section);
    void ParseShader(const ucl::Ucl& section, const std::string& sectionName, ShaderData& shader);
    void ParseParameters(const ucl::Ucl& section, const std::string& sectionName, ShaderData& shader);

private:
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
