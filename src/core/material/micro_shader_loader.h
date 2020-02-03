#pragma once

#include <ucl++.h>
#include "core/common/ctor.h"
#include "core/material/material_builder_desc.h"


class MicroShaderLoader : Fixed {
public:
//     struct Source {
//         std::string name;
//         std::string vs;
//         std::string ps;
//         std::string gs;
//     };

private:
    struct InputType {
        std::string type;
        std::string semantic;
    };

//     struct ShaderData {
//         void Append(const ShaderData& other);
//         std::string GenParametersToStr(const MaterialBuilderDesc& desc);

//         std::set<std::string> includes;
//         std::set<std::string> textures2D;
//         std::set<std::string> cbuffers;
//         std::map<std::string, InputType> inputs;
//         Mixing mixing = Mixing::Add;
//         bool isEmpty = true;
//         std::string source;
//     };

    struct PixelData {
        bool isEmpty = true;
        std::string entrypoint;
        int64_t order = 0;
        bool isOverride = false;
        std::vector<std::string> includes;
        std::vector<std::string> textures2D;
        std::map<std::string, InputType> psInput;
        std::map<std::string, std::string> psOutput;
        std::map<std::string, std::string> psLocal;
        std::map<std::string, std::string> cbuffers;
        std::string source;
    };

    struct GeometryData {
        bool isEmpty = true;
        std::vector<std::string> includes;
        std::vector<std::string> gsOutput;
        std::vector<std::string> gsInput;
        std::vector<std::string> textures2D;
        std::map<std::string, std::string> cbuffers;
        std::string source;
    };

    struct Microshader {
        bool isEmpty = true;
//         bool autogen = false;
        std::string name;
        std::string group;
//         uint32_t groupID = 0;

//         ShaderData vs;
        PixelData ps;
        GeometryData gs;
    };

public:
    MicroShaderLoader() = default;
    ~MicroShaderLoader() = default;

    void Load(const MaterialBuilderDesc& desc);
    // uint64_t GetMask(const std::string& name) const;
    // Source GetSources(uint64_t mask) const;

private:
    void ParseMicroshader(const ucl::Ucl& section);
    void ParsePixel(const ucl::Ucl& section, const std::string& sectionName, PixelData& data);
    void ParseGeometry(const ucl::Ucl& section, const std::string& sectionName, GeometryData& data);
    void ParseKV(const ucl::Ucl& section, const std::string& sectionName, std::map<std::string, std::string>& data);
    void ParseInputs(const ucl::Ucl& section, const std::string& sectionName, std::map<std::string, InputType>& data);

private:
    MaterialBuilderDesc m_desc;

    Microshader m_root;
    // namedMicroShaderID => namedMicroShader
    // std::vector<Microshader> m_namedMicroShaders;
    // // namedMicroShaderName => namedMicroShaderID
    // std::map<std::string, uint32_t> m_namedMicroShaderIDs;
    // // groupID => defaultMicroShader
    // std::vector<Microshader> m_defaultMicroShaders;
    // // groupName => groupID
    // std::map<std::string, uint32_t> m_groupIDs;
};
