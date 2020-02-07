#pragma once

#include <vector>
#include <ucl++.h>
#include "core/material/material_builder_desc.h"


namespace msh {

using Item = std::string;

class Items {
public:
    Items() = default;
    ~Items() = default;

    void Parse(const ucl::Ucl& section);
    void Append(const Items& other);
    void Generate(const std::function<void (const Item&, std::string&)>& func, std::string& out);
    void GenerateIncludes(std::string& out);
    void GenerateTextures(const std::string& samplerSuffix, std::string& out);

private:
    std::vector<std::string> m_data;
};

struct Decl {
    Decl() = default;
    Decl(const std::string& name, const std::string& type);

    bool operator <(const Decl& other) const;
    void Swap(Decl& other);

    std::string name;
    std::string type;
};

class Decls {
public:
    Decls() = default;
    ~Decls() = default;

    void Parse(const ucl::Ucl& section);
    void Append(const Decls& other);
    void Generate(const std::function<void (const Decl&, std::string&)>& func, std::string& out);
    void GenerateStruct(const std::string& name, std::string& out);
    void GenerateCbuffer(const std::function<std::string (const std::string&)>& cbufferNameGenerator, std::string& out);

private:
    std::vector<Decl> m_data;
};

struct SemanticDecl {
    SemanticDecl() = default;
    SemanticDecl(const std::string& name, const std::string& type, const std::string& semantic);

    bool operator <(const SemanticDecl& other) const;
    void Swap(SemanticDecl& other);

    std::string name;
    std::string type;
    std::string semantic;
};

class SemanticDecls {
public:
    SemanticDecls() = default;
    ~SemanticDecls() = default;

    void Parse(const ucl::Ucl& section);
    void Append(const SemanticDecls& other);
    void Generate(const std::function<void (const SemanticDecl&, std::string&)>& func, std::string& out);
    void GenerateStruct(const std::string& name, std::string& out);

private:
    std::vector<SemanticDecl> m_data;
};

struct PixelMicroshader {
    void Parse(const ucl::Ucl& section);
    void Append(const PixelMicroshader* value);
    void Generate(const MaterialBuilderDesc& desc, std::string& out);

    bool isEmpty = true;
    std::string entrypoint;
    int64_t order = 0;
    bool isOverride = false;
    Items includes;
    Items textures2D;
    SemanticDecls psInput;
    SemanticDecls psOutput;
    Decls psLocal;
    Decls cbuffers;
    std::string source;
};

class PixelShader {
public:
    PixelShader() = default;
    ~PixelShader() = default;

    void Append(const PixelMicroshader* value);
    void Generate(const MaterialBuilderDesc& desc, std::string& out);

private:
    bool m_isOverrideFound = false;
    std::vector<const PixelMicroshader*> m_data;
};

struct GeometryMicroshader {
    void Parse(const ucl::Ucl& section);

    bool isEmpty = true;
    Items includes;
    Items gsOutput;
    Items gsInput;
    Items textures2D;
    Decls cbuffers;
    std::string source;
};

class GeometryShader {
public:
    GeometryShader() = default;
    ~GeometryShader() = default;

    void Append(const GeometryMicroshader* value);
    void Generate(const MaterialBuilderDesc& desc, std::string& out);

private:
    GeometryMicroshader m_data;
};

}
