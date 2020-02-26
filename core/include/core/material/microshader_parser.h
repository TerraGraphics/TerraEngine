#pragma once

#include <map>
#include <string>
#include <vector>
#include <cstdint>

#include "core/material/microshader_types.h"

namespace ucl {
    class Ucl;
}
struct MaterialBuilderDesc;
namespace msh {

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
    uint8_t GetOutputNumber() const { return m_outputNumber; }

    void Append(const PixelMicroshader* value);
    void Generate(const MaterialBuilderDesc& desc, std::string& out);

private:
    bool m_isOverrideFound = false;
    uint8_t m_outputNumber = 0;
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
    bool IsEmpty() const { return m_data.isEmpty; }

    void Append(const GeometryMicroshader& value);
    void Generate(const MaterialBuilderDesc& desc, const SemanticDecls& output, std::string& out);

private:
    SemanticDecls m_input;
    GeometryMicroshader m_data;
};

struct VertexMicroshader {
    void Parse(const ucl::Ucl& section, const std::string& baseName);
    void Append(const VertexMicroshader* value);
    void Generate(const MaterialBuilderDesc& desc, SemanticDecls input, SemanticDecls output, std::string& out);

    bool isEmpty = true;
    std::string entrypoint;
    int64_t order = 0;
    Items includes;
    Items vsInput;
    Items textures2D;
    Decls cbuffers;
    std::string source;
};

class VertexShader {
public:
    VertexShader() = default;
    ~VertexShader() = default;

    void Append(const std::map<std::string, VertexMicroshader>& value);
    void Generate(const MaterialBuilderDesc& desc, const SemanticDecls& input, const SemanticDecls& output, std::string& out);

private:
    // VSOutput => VertexMicroshader
    std::map<std::string, VertexMicroshader> m_data;
};

}
