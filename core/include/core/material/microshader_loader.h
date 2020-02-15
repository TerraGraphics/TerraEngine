#pragma once

#include "core/common/ctor.h"
#include "core/material/microshader_parser.h"


class MicroshaderLoader : Fixed {
public:
    struct Source {
        std::string name;
        std::string vs;
        std::string ps;
        std::string gs;
        uint8_t gsOutputNumber = 0;
    };

private:
    struct Microshader {
        bool isEmpty = true;
        bool isRoot = false;
        std::string name;
        std::string group;
        uint32_t groupID = 0;

        // VSOutput => VertexMicroshader
        std::map<std::string, msh::VertexMicroshader> vs;
        msh::PixelMicroshader ps;
        msh::GeometryMicroshader gs;
    };

public:
    MicroshaderLoader() = default;
    ~MicroshaderLoader() = default;

    void Load(const MaterialBuilderDesc& desc);
    uint64_t GetMask(const std::string& name) const;
    Source GetSources(uint64_t mask, const msh::SemanticDecls& vertexInput) const;

private:
    bool ReadMicroshader(const std::filesystem::path& filepath, const std::string& requiredExtension, ucl_object_t* schema, ucl::Ucl& section);
    void ParseMicroshader(const ucl::Ucl& section, Microshader& ms);

private:
    MaterialBuilderDesc m_desc;

    Microshader m_root;
    // microshaderID => microshader
    std::vector<Microshader> m_microshaders;
    // MicroshaderName => MicroshaderID
    std::map<std::string, uint32_t> m_microshaderIDs;
    // Mask => Source
    mutable std::map<uint64_t, Source> m_cache;
};
