#include "core/material/microshader_parser.h"

#include <fmt/format.h>
#include "core/common/exception.h"


using namespace msh;

void Items::Parse(const ucl::Ucl& section) {
    m_data.clear();
    for (const auto& it: section) {
        m_data.push_back(it.string_value());
    }
}

void Items::Append(const Items& other) {
    m_data.insert(m_data.cend(), other.m_data.cbegin(), other.m_data.cend());
}

void Items::Generate(const std::function<void (const Item&, std::string&)>& func, std::string& out) {
    if (m_data.empty()) {
        return;
    }
    std::sort(m_data.begin(), m_data.end());
    std::string last;
    for (const auto& it: m_data) {
        if (it != last) {
            func(it, out);
            last = it;
        }
    }
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

void Decls::Parse(const ucl::Ucl& section) {
    m_data.clear();
    for (const auto& it: section) {
        m_data.emplace_back(it.key(), it.string_value());
    }
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

void SemanticDecls::Parse(const ucl::Ucl& section) {
    m_data.clear();
    for (const auto& it: section) {
        m_data.emplace_back(it.key(), it[0].string_value(), it[1].string_value());
    }
}

void SemanticDecls::Append(const SemanticDecls& other) {
    m_data.insert(m_data.cend(), other.m_data.cbegin(), other.m_data.cend());
}

void SemanticDecls::Generate(const std::function<void (const SemanticDecl&, std::string&)>& func, std::string& out) {
    if (m_data.empty()) {
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
}

void SemanticDecls::GenerateStruct(const std::string& name, std::string& out) {
    out.append("struct " + name + " {\n");
    Generate([](const SemanticDecl& d, std::string& out){
        out.append(fmt::format("    {} {} : {};\n", d.type, d.name, d.semantic));
    }, out);
    out.append("};\n");
}


void PixelMicroshader::Parse(const ucl::Ucl& section) {
    isEmpty = false;
    for (const auto &it: section) {
        if (it.key() == "entrypoint") {
            entrypoint = it.string_value();
            if (entrypoint == "main") {
                throw EngineError("entrypoints with the name 'main' is disabled");
            }
        } else if (it.key() == "order") {
            order = it.int_value();
        } else if (it.key() == "override") {
            isOverride = it.bool_value();
        } else if (it.key() == "include") {
            includes.Parse(it);
        } else if (it.key() == "PSOutput") {
            psOutput.Parse(it);
        } else if (it.key() == "PSInput") {
            psInput.Parse(it);
        } else if (it.key() == "PSLocal") {
            psLocal.Parse(it);
        } else if (it.key() == "cbuffers") {
            cbuffers.Parse(it);
        } else if (it.key() == "textures2D") {
            textures2D.Parse(it);
        } else if (it.key() == "source") {
            source = it.string_value();
        } else {
            throw EngineError("unknown section: {}.{} with data: {}", section.key(), it.key(), it.dump());
        }
    }
}

void PixelMicroshader::Append(const PixelMicroshader* other) {
    includes.Append(other->includes);
    textures2D.Append(other->textures2D);
    psInput.Append(other->psInput);
    psOutput.Append(other->psOutput);
    psLocal.Append(other->psLocal);
    cbuffers.Append(other->cbuffers);
    source.append(other->source + "\n");
}

void PixelMicroshader::Generate(const MaterialBuilderDesc& desc, std::string& out) {
    includes.GenerateIncludes(out);
    psOutput.GenerateStruct("PSOutput", out);
    psInput.GenerateStruct("PSInput", out);
    psLocal.GenerateStruct("PSLocal", out);
    cbuffers.GenerateCbuffer(desc.cbufferNameGenerator, out);
    textures2D.GenerateTextures(desc.samplerSuffix, out);
    out.append(source);
}

void PixelShader::Append(const PixelMicroshader* value) {
    if (value->isEmpty) {
        return;
    }
    if (value->isOverride && m_isOverrideFound) {
        throw EngineError("flag 'isOverride' for pixel shader is duplicated");
    }
    if (value->isOverride) {
        m_data = {value};
        m_isOverrideFound = true;
    } else if (!m_isOverrideFound) {
        m_data.push_back(value);
    }
}

void PixelShader::Generate(const MaterialBuilderDesc& desc, std::string& out) {
    if (m_data.empty()) {
        throw EngineError("not found pixel microshaders");
    }

    std::sort(m_data.begin(), m_data.end(),
        [](const PixelMicroshader* a, const PixelMicroshader* b) -> bool { return a->order > b->order; });

    PixelMicroshader base;
    std::string entrypointsCall;
    for (const auto* ps: m_data) {
        entrypointsCall.append("    " + ps->entrypoint + "(psIn, psLocal, psOut);\n");
        base.Append(ps);
    }

    base.Generate(desc, out);
    out.append("void main(in PSInput psIn, PSOutput psOut) {\nPSLocal psLocal;\n");
    out.append(entrypointsCall);
    out.append("}");
}

void GeometryMicroshader::Parse(const ucl::Ucl& section) {
    isEmpty = false;
    for (const auto &it: section) {
        if (it.key() == "include") {
            includes.Parse(it);
        } else if (it.key() == "GSOutput") {
            gsOutput.Parse(it);
        } else if (it.key() == "GSInput") {
            gsInput.Parse(it);
        } else if (it.key() == "cbuffers") {
            cbuffers.Parse(it);
        } else if (it.key() == "textures2D") {
            textures2D.Parse(it);
        } else if (it.key() == "source") {
            source = it.string_value();
        } else {
            throw EngineError("unknown section: {}.{} with data: {}", section.key(), it.key(), it.dump());
        }
    }
}

void GeometryShader::Append(const GeometryMicroshader* value) {
    if (value->isEmpty) {
        return;
    }

    if (!m_data.isEmpty) {
        throw EngineError("found double geometry shader");
    }

    m_data = *value;
}

void GeometryShader::Generate(const MaterialBuilderDesc& desc, std::string& out) {
}
