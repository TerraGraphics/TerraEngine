#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <functional>


namespace msh {

using Item = std::string;

class Items {
public:
    Items() = default;
    ~Items() = default;

    bool IsPreProcessed() const { return m_isPreProcessed; }
    const std::vector<Item> GetData() const { return m_data; }
    std::string JoinNames(const std::string& sep) const;

    void SetData(std::vector<Item>&& data);
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

    void SetData(std::vector<Decl>&& data);
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
    SemanticDecls(std::vector<SemanticDecl>&& data);
    ~SemanticDecls() = default;

    const std::vector<SemanticDecl> GetDataCopy() const { return m_data; }
    const std::vector<SemanticDecl>& GetDataRef() const { return m_data; }
    size_t GetSize() const { return m_data.size(); }
    std::string JoinNames(const std::string& sep) const;

    void SetData(std::vector<SemanticDecl>&& data);
    void SetIsPreProcessed(bool value);
    void Append(const SemanticDecls& other);
    void Generate(const std::function<void (const SemanticDecl&, std::string&)>& func, std::string& out);
    void GenerateStruct(const std::string& name, std::string& out);
    bool IsEqual(const Items& value) const;

private:
    bool m_isPreProcessed = false;
    std::vector<SemanticDecl> m_data;
};

}
