#include "core/material/microshader_parser.h"

#include <utility>
#include <algorithm>
#include <type_traits>

#include "core/dg/ucl.h"
#include "core/common/exception.h"
#include "core/material/material_builder_desc.h"


using namespace msh;


static void Parse(const ucl::Ucl& section, Items& out) {
    std::vector<Item> data;
    for (const auto& it: section) {
        data.push_back(it.string_value());
    }
    out.SetData(std::move(data));
}

static void Parse(const ucl::Ucl& section, Decls& out) {
    std::vector<Decl> data;
    for (const auto& it: section) {
        data.emplace_back(it.key(), it.string_value());
    }
    out.SetData(std::move(data));
}

static void Parse(const ucl::Ucl& section, SemanticDecls& out) {
    std::vector<SemanticDecl> data;
    for (const auto& it: section) {
        data.emplace_back(it.key(), it[0].string_value(), it[1].string_value());
    }
    out.SetData(std::move(data));
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
            ::Parse(it, includes);
        } else if (it.key() == "PSOutput") {
            ::Parse(it, psOutput);
        } else if (it.key() == "PSInput") {
            ::Parse(it, psInput);
        } else if (it.key() == "PSLocal") {
            ::Parse(it, psLocal);
        } else if (it.key() == "cbuffers") {
            ::Parse(it, cbuffers);
        } else if (it.key() == "textures2D") {
            ::Parse(it, textures2D);
        } else if (it.key() == "source") {
            source = it.string_value();
        } else {
            throw EngineError("unknown section: {}.{} with data: {}", section.key(), it.key(), it.dump());
        }
    }
}

void PixelMicroshader::Append(const PixelMicroshader* value) {
    includes.Append(value->includes);
    textures2D.Append(value->textures2D);
    psInput.Append(value->psInput);
    psOutput.Append(value->psOutput);
    psLocal.Append(value->psLocal);
    cbuffers.Append(value->cbuffers);
    source.append(value->source + "\n");
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
        [](const PixelMicroshader* a, const PixelMicroshader* b) -> bool { return a->order < b->order; });

    PixelMicroshader base;
    for (const auto* ps: m_data) {
        base.Append(ps);
    }

    base.Generate(desc, out);
    out.append("void main(in PSInput psIn, out PSOutput psOut) {\n    PSLocal psLocal;\n");
    for (const auto* ps: m_data) {
        out.append("    " + ps->entrypoint + "(psIn, psLocal, psOut);\n");
    }
    out.append("}");

    m_outputNumber = static_cast<uint8_t>(base.psOutput.GetSize());
    m_input = base.psInput;
}

void GeometryMicroshader::Parse(const ucl::Ucl& section) {
    isEmpty = false;
    for (const auto &it: section) {
        if (it.key() == "include") {
            ::Parse(it, includes);
        } else if (it.key() == "GSOutput") {
            ::Parse(it, gsOutput);
        } else if (it.key() == "GSInput") {
            ::Parse(it, gsInput);
        } else if (it.key() == "cbuffers") {
            ::Parse(it, cbuffers);
        } else if (it.key() == "textures2D") {
            ::Parse(it, textures2D);
        } else if (it.key() == "source") {
            source = it.string_value();
        } else {
            throw EngineError("unknown section: {}.{} with data: {}", section.key(), it.key(), it.dump());
        }
    }
}

void GeometryMicroshader::Generate(const MaterialBuilderDesc& desc, SemanticDecls output, std::string& out) {
    includes.GenerateIncludes(out);
    output.GenerateStruct("GSOutput", out);
    gsInput.GenerateStruct("GSInput", out);
    cbuffers.GenerateCbuffer(desc.cbufferNameGenerator, out);
    textures2D.GenerateTextures(desc.samplerSuffix, out);
    out.append(source);
}

void GeometryShader::Append(const GeometryMicroshader& value) {
    if (value.isEmpty) {
        return;
    }

    if (!m_data.isEmpty) {
        throw EngineError("found double geometry shader");
    }

    m_data = value;
}

void GeometryShader::Generate(const MaterialBuilderDesc& desc, const SemanticDecls& output, std::string& out) {
    if (m_data.isEmpty) {
        return;
    }

    m_data.gsOutput.GenerateNone();
    if (!output.IsEqual(m_data.gsOutput)) {
        throw EngineError("pixel and geometric shaders are not data-compatible");
    }

    m_data.Generate(desc, output, out);

    m_input = m_data.gsInput;
}

void VertexMicroshader::Parse(const ucl::Ucl& section, const std::string& baseName) {
    isEmpty = false;
    for (const auto &it: section) {
        if (it.key() == "entrypoint") {
            entrypoint = it.string_value();
            if (entrypoint == "main") {
                throw EngineError("entrypoints with the name 'main' is disabled");
            }
        } else if (it.key() == "order") {
            order = it.int_value();
        } else if (it.key() == "include") {
            ::Parse(it, includes);
        } else if (it.key() == "VSInput") {
            ::Parse(it, vsInput);
        } else if (it.key() == "cbuffers") {
            ::Parse(it, cbuffers);
        } else if (it.key() == "textures2D") {
            ::Parse(it, textures2D);
        } else if (it.key() == "source") {
            source = it.string_value();
        } else {
            throw EngineError("unknown section: {}.{}.{} with data: {}", baseName, section.key(), it.key(), it.dump());
        }
    }
}

void VertexMicroshader::Append(const VertexMicroshader* value) {
    includes.Append(value->includes);
    vsInput.Append(value->vsInput);
    textures2D.Append(value->textures2D);
    cbuffers.Append(value->cbuffers);
    source.append(value->source + "\n");
}

void VertexMicroshader::Generate(const MaterialBuilderDesc& desc, SemanticDecls input, SemanticDecls output, std::string& out) {
    includes.GenerateIncludes(out);
    output.GenerateStruct("VSOutput", out);
    input.SetIsPreProcessed(true);
    input.GenerateStruct("VSInput", out);
    cbuffers.GenerateCbuffer(desc.cbufferNameGenerator, out);
    textures2D.GenerateTextures(desc.samplerSuffix, out);
    out.append(source);
}

void VertexShader::Append(const std::map<std::string, VertexMicroshader>& value) {
    for (const auto& [key, vs]: value) {
        if (vs.isEmpty) {
            continue;
        }
        const auto it = m_data.find(key);
        if (it == m_data.cend()) {
            m_data[key] = vs;
        } else if (it->second.order < vs.order) {
            m_data[key] = vs;
        }
    }
}

void VertexShader::Generate(const MaterialBuilderDesc& desc, const SemanticDecls& input, const SemanticDecls& output, std::string& out) {
    std::vector<const VertexMicroshader*> gendata;
    for (const auto& item : output.GetDataRef()) {
        const auto it = m_data.find(item.name);
        if (it == m_data.cend()) {
            throw EngineError("vertex and pixel or geometric shaders are not data-compatible");
        }
        gendata.push_back(&it->second);
    }

    if (gendata.empty()) {
        throw EngineError("not found vertex microshaders");
    }

    std::sort(gendata.begin(), gendata.end(),
        [](const VertexMicroshader* a, const VertexMicroshader* b) -> bool { return a->order < b->order; });

    VertexMicroshader base;
    for (const auto* vs: gendata) {
        base.Append(vs);
    }

    for (const auto& name : base.vsInput.GetData()) {
        bool isFound = false;
        for (const auto& item : input.GetDataRef()) {
            if (name == item.name) {
                isFound = true;
                break;
            }
        }
        if (!isFound) {
            throw EngineError("vertex shaders input ({}) and layoutInput ({}) are not data-compatible",
                base.vsInput.JoinNames(", "), input.JoinNames(", "));
        }
    }

    base.Generate(desc, input, output, out);
    out.append("void main(in VSInput vsIn, out VSOutput vsOut) {\n");
    for (const auto* vs: gendata) {
        out.append("    " + vs->entrypoint + "(vsIn, vsOut);\n");
    }
    out.append("}");
}
