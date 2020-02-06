#pragma once

#include "core/common/ctor.h"
#include "core/material/microshader_parser.h"
#include "core/material/material_builder_desc.h"


class MicroshaderLoader : Fixed {
public:
    struct Source {
        std::string name;
        std::string vs;
        std::string ps;
        std::string gs;
    };

private:
    struct VertexData {
        bool isEmpty = true;
        std::string entrypoint;
        int64_t order = 0;
        bool isOverride = false;
        msh::Items includes;
        msh::Items vsInput;
        msh::Items textures2D;
        msh::Decls cbuffers;
        std::string source;
    };

    struct PixelData {
        void Append(const PixelData* other, std::vector<std::string>& entrypoints);
        std::string Generate(const std::vector<std::string>& entrypoints, const MaterialBuilderDesc& desc);

        bool isEmpty = true;
        std::string entrypoint;
        int64_t order = 0;
        bool isOverride = false;
        msh::Items includes;
        msh::Items textures2D;
        msh::SemanticDecls psInput;
        msh::Decls psOutput;
        msh::Decls psLocal;
        msh::Decls cbuffers;
        std::string source;
    };

    struct GeometryData {
        std::string Generate(const msh::SemanticDecls& psInput);

        bool isEmpty = true;
        msh::Items includes;
        msh::Items gsOutput;
        msh::Items gsInput;
        msh::Items textures2D;
        msh::Decls cbuffers;
        std::string source;
    };

    struct Microshader {
        bool isEmpty = true;
        bool isRoot = false;
        std::string name;
        std::string group;
        uint32_t groupID = 0;

        // VSOutput => VertexData
        std::map<std::string, VertexData> vs;
        PixelData ps;
        GeometryData gs;
    };

public:
    MicroshaderLoader() = default;
    ~MicroshaderLoader() = default;

    void Load(const MaterialBuilderDesc& desc);
    uint64_t GetMask(const std::string& name) const;
    Source GetSources(uint64_t mask) const;

private:
    bool ReadMicroshader(const std::filesystem::path& filepath, const std::string& requiredExtension, ucl_object_t* schema, ucl::Ucl& section);
    void ParseMicroshader(const ucl::Ucl& section, Microshader& ms);
    void ParseVertex(const ucl::Ucl& section, const std::string& sectionName, std::map<std::string, VertexData>& data);
    void ParseVertexItem(const ucl::Ucl& section, const std::string& sectionName, VertexData& data);
    void ParsePixel(const ucl::Ucl& section, const std::string& sectionName, PixelData& data);
    void ParseGeometry(const ucl::Ucl& section, const std::string& sectionName, GeometryData& data);

private:
    MaterialBuilderDesc m_desc;

    Microshader m_root;
    // microshaderID => microshader
    std::vector<Microshader> m_microshaders;
    // MicroshaderName => MicroshaderID
    std::map<std::string, uint32_t> m_microshaderIDs;
};
