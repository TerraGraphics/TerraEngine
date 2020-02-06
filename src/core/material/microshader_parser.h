#pragma once

#include <string>
#include <vector>
#include <ucl++.h>
#include <functional>


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

}
