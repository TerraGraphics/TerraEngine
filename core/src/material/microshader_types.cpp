#include "core/material/microshader_types.h"

#include <fmt/format.h>
#include "core/common/exception.h"


using namespace msh;

std::string Items::JoinNames(const std::string& sep) const {
    std::string result;
    bool isFirst = true;
    for (const auto& it: m_data) {
        if (!isFirst) {
            result.append(sep);
        }
        isFirst = false;
        result.append(it);
    }

    return result;
}

void Items::SetData(std::vector<Item>&& data) {
    m_isPreProcessed = false;
    m_data.swap(data);
}

void Items::Append(const Items& other) {
    m_isPreProcessed = false;
    m_data.insert(m_data.cend(), other.m_data.cbegin(), other.m_data.cend());
}

void Items::Generate(const std::function<void (const Item&, std::string&)>& func, std::string& out) {
    if (m_data.empty()) {
        return;
    }

    if (m_isPreProcessed) {
        for (const auto& it: m_data) {
            func(it, out);
        }
        return;
    }

    std::sort(m_data.begin(), m_data.end());

    auto last = m_data.begin();
    func(*last, out);

    for(auto it=last + 1; it!=m_data.end(); ++it) {
        if (*it != *last) {
            func(*it, out);
            last++;
            if (last != it) {
                last->swap(*it);
            }
        }
    }

    m_data.resize(std::distance(m_data.begin(), last) + 1);
    m_isPreProcessed = true;
}

void Items::GenerateNone() {
    std::string out;
    Generate([](const Item&, std::string&){}, out);
}

void Items::GenerateIncludes(std::string& out) {
    Generate([](const Item& item, std::string& out){
        out.append("#include \"" + item + "\"\n");
    }, out);
}

void Items::GenerateTextures(const std::string& samplerSuffix, std::string& out) {
    Generate([&samplerSuffix](const Item& item, std::string& out){
        out.append(fmt::format("Texture2D {0};\nSamplerState {0}{1};\n", item, samplerSuffix));
    }, out);
}

Decl::Decl(const std::string& name, const std::string& type)
    : name(name)
    , type(type) {

}

bool Decl::operator <(const Decl& other) const {
    return (std::tie(name, type) < std::tie(other.name, other.type));
}

void Decl::Swap(Decl& other) {
    std::swap(name, other.name);
    std::swap(type, other.type);
}

void Decls::SetData(std::vector<Decl>&& data) {
    m_data.swap(data);
}

void Decls::Append(const Decls& other) {
    m_data.insert(m_data.cend(), other.m_data.cbegin(), other.m_data.cend());
}

void Decls::Generate(const std::function<void (const Decl&, std::string&)>& func, std::string& out) {
    if (m_data.empty()) {
        return;
    }
    std::sort(m_data.begin(), m_data.end());
    std::string lastName;
    std::string lastType;
    for (const auto& it: m_data) {
        if (it.name != lastName) {
            func(it, out);
            lastName = it.name;
            lastType = it.type;
        } else if (it.type != lastType) {
            throw EngineError("different types for one name '{}': '{}' and '{}'", it.name, it.type, lastType);
        }
    }
}

void Decls::GenerateStruct(const std::string& name, std::string& out) {
    out.append("struct " + name + " {\n");
    Generate([](const Decl& d, std::string& out){
        out.append(fmt::format("    {} {};\n", d.type, d.name));
    }, out);
    out.append("};\n");
}

void Decls::GenerateCbuffer(const std::function<std::string (const std::string&)>& cbufferNameGenerator, std::string& out) {
    Generate([&cbufferNameGenerator](const Decl& d, std::string& out){
        out.append(fmt::format("cbuffer {} {{ {} {}; }};\n", cbufferNameGenerator(d.name), d.type, d.name));
    }, out);
}

SemanticDecl::SemanticDecl(const std::string& name, const std::string& type, const std::string& semantic)
    : name(name)
    , type(type)
    , semantic(semantic) {

}

bool SemanticDecl::operator <(const SemanticDecl& other) const {
    return (std::tie(name, type, semantic) < std::tie(other.name, other.type, other.semantic));
}

void SemanticDecl::Swap(SemanticDecl& other) {
    std::swap(name, other.name);
    std::swap(type, other.type);
    std::swap(semantic, other.semantic);
}

std::string SemanticDecls::JoinNames(const std::string& sep) const {
    std::string result;
    bool isFirst = true;
    for (const auto& it: m_data) {
        if (!isFirst) {
            result.append(sep);
        }
        isFirst = false;
        result.append(it.name);
    }

    return result;
}

void SemanticDecls::SetData(std::vector<SemanticDecl>&& data) {
    m_isPreProcessed = false;
    m_data.swap(data);
}

void SemanticDecls::SetIsPreProcessed(bool value) {
    m_isPreProcessed = value;
}

void SemanticDecls::Append(const SemanticDecls& other) {
    m_isPreProcessed = false;
    m_data.insert(m_data.cend(), other.m_data.cbegin(), other.m_data.cend());
}

void SemanticDecls::Generate(const std::function<void (const SemanticDecl&, std::string&)>& func, std::string& out) {
    if (m_data.empty()) {
        return;
    }

    if (m_isPreProcessed) {
        for(const auto& it: m_data) {
            func(it, out);
        }
        return;
    }

    std::sort(m_data.begin(), m_data.end());

    auto last = m_data.begin();
    func(*last, out);

    for(auto it=last + 1; it!=m_data.end(); ++it) {
        if (it->name != last->name) {
            func(*it, out);
            last++;
            if (last != it) {
                last->Swap(*it);
            }
        } else if (it->type != last->type) {
            throw EngineError("different types for one name '{}': '{}' and '{}'", it->name, it->type, last->type);
        } else if (it->semantic != last->semantic) {
            throw EngineError("different semantics for one name '{}': '{}' and '{}'", it->name, it->semantic, last->semantic);
        }
    }

    m_data.resize(std::distance(m_data.begin(), last) + 1);
    m_isPreProcessed = true;
}

void SemanticDecls::GenerateStruct(const std::string& name, std::string& out) {
    out.append("struct " + name + " {\n");
    Generate([](const SemanticDecl& d, std::string& out){
        out.append(fmt::format("    {} {} : {};\n", d.type, d.name, d.semantic));
    }, out);
    out.append("};\n");
}

bool SemanticDecls::IsEqual(const Items& value) const {
    if (!m_isPreProcessed) {
        throw EngineError("local data for function SemanticDecls::IsEqual is not pre-processed");
    }
    if (!value.IsPreProcessed()) {
        throw EngineError("input data for function SemanticDecls::IsEqual is not pre-processed");
    }

    const auto& data = value.GetData();
    if (data.size() != m_data.size()) {
        return false;
    }

    auto valueIt = data.cbegin();
    for (const auto& localIt: m_data) {
        if (*valueIt != localIt.name) {
            return false;
        }
        ++valueIt;
    }

    return true;
}
