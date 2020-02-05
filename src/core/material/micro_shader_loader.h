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
    struct Decl {
        Decl(const std::string& name, const std::string& type);
        bool operator <(const Decl& other) const;
        static void JoinUniq(std::vector<Decl>& arr, std::string& out);
        static void JoinUniq(std::vector<Decl>& arr, std::string& out, const std::function<std::string (const Decl&)>& generator);

        std::string name;
        std::string type;
    };

    struct DeclWithSemantic {
        DeclWithSemantic(const std::string& name, const std::string& type, const std::string& semantic);
        bool operator <(const DeclWithSemantic& other) const;
        static void JoinUniq(std::vector<DeclWithSemantic>& arr, std::string& out);

        std::string name;
        std::string type;
        std::string semantic;
    };

    struct VertexData {
        bool isEmpty = true;
        std::string entrypoint;
        int64_t order = 0;
        bool isOverride = false;
        std::vector<std::string> includes;
        std::vector<std::string> vsInput;
        std::vector<std::string> textures2D;
        std::vector<Decl> cbuffers;
        std::string source;
    };

    struct PixelData {
        void Append(const PixelData* other, std::vector<std::string>& entrypoints);
        std::string Generate(const std::vector<std::string>& entrypoints, const MaterialBuilderDesc& desc);

        bool isEmpty = true;
        std::string entrypoint;
        int64_t order = 0;
        bool isOverride = false;
        std::vector<std::string> includes;
        std::vector<std::string> textures2D;
        std::vector<DeclWithSemantic> psInput;
        std::vector<Decl> psOutput;
        std::vector<Decl> psLocal;
        std::vector<Decl> cbuffers;
        std::string source;
    };

    struct GeometryData {
        bool isEmpty = true;
        std::vector<std::string> includes;
        std::vector<std::string> gsOutput;
        std::vector<std::string> gsInput;
        std::vector<std::string> textures2D;
        std::vector<Decl> cbuffers;
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
    MicroShaderLoader() = default;
    ~MicroShaderLoader() = default;

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
    void ParseArray(const ucl::Ucl& section, std::vector<std::string>& data);
    void ParseDecl(const ucl::Ucl& section, std::vector<Decl>& data);
    void ParseDeclWithSemantic(const ucl::Ucl& section, std::vector<DeclWithSemantic>& data);

private:
    MaterialBuilderDesc m_desc;

    Microshader m_root;
    // microshaderID => microshader
    std::vector<Microshader> m_microshaders;
    // MicroshaderName => MicroshaderID
    std::map<std::string, uint32_t> m_microshaderIDs;
};
