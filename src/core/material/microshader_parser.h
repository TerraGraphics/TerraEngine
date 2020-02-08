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

    const std::vector<Item> GetData() const { return m_data; }
    bool IsPreProcessed() const { return m_isPreProcessed; }

    void Parse(const ucl::Ucl& section);
    void Append(const Items& other);
    void Generate(const std::function<void (const Item&, std::string&)>& func, std::string& out);
    void GenerateNone();
    void GenerateIncludes(std::string& out);
    void GenerateTextures(const std::string& samplerSuffix, std::string& out);

private:
    bool m_isPreProcessed = false;
    std::vector<Item> m_data;
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
    bool IsEqual(const Items& value) const;

private:
    bool m_isPreProcessed = false;
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

    const SemanticDecls& GetInput() const { return m_input; }

    void Append(const PixelMicroshader* value);
    void Generate(const MaterialBuilderDesc& desc, std::string& out);

private:
    bool m_isOverrideFound = false;
    SemanticDecls m_input;
    std::vector<const PixelMicroshader*> m_data;
};

struct GeometryMicroshader {
    void Parse(const ucl::Ucl& section);
    void Generate(const MaterialBuilderDesc& desc, SemanticDecls output, std::string& out);

    bool isEmpty = true;
    Items includes;
    Items gsOutput;
    SemanticDecls gsInput;
    Items textures2D;
    Decls cbuffers;
    std::string source;
};

class GeometryShader {
public:
    GeometryShader() = default;
    ~GeometryShader() = default;

    const SemanticDecls& GetInput() const { return m_input; }

    void Append(const GeometryMicroshader* value);
    void Generate(const MaterialBuilderDesc& desc, const SemanticDecls& output, std::string& out);

private:
    SemanticDecls m_input;
    GeometryMicroshader m_data;
};

}
