#include "core/material/micro_shader_loader.h"

#include "core/common/exception.h"


static void JoinUniq(std::vector<std::string>& arr, std::string& out) {
    if (arr.empty()) {
        return;
    }
    std::sort(arr.begin(), arr.end());
    std::string last;
    for (const auto& it: arr) {
        if (it != last) {
            out.append(it + "\n");
            last = it;
        }
    }
}

MicroShaderLoader::Decl::Decl(const std::string& name, const std::string& type)
    : name(name)
    , type(type) {

}

bool MicroShaderLoader::Decl::operator <(const Decl& other) const {
    return (std::tie(name, type) < std::tie(other.name, other.type));
}

void MicroShaderLoader::Decl::JoinUniq(std::vector<Decl>& arr, std::string& out) {
    return JoinUniq(arr, out, [](const Decl& d) {
        return fmt::format("{} {};\n", d.type, d.name);
    });
}

void MicroShaderLoader::Decl::JoinUniq(std::vector<Decl>& arr, std::string& out, const std::function<std::string (const Decl&)>& generator) {
    if (arr.empty()) {
        return;
    }
    std::sort(arr.begin(), arr.end());
    std::string lastName;
    std::string lastType;
    for (const auto& it: arr) {
        if (it.name != lastName) {
            out.append(generator(it));
            lastName = it.name;
            lastType = it.type;
        } else if (it.type != lastType) {
            throw EngineError("different types for one name '{}': '{}' and '{}'", it.name, it.type, lastType);
        }
    }
}

MicroShaderLoader::DeclWithSemantic::DeclWithSemantic(const std::string& name, const std::string& type, const std::string& semantic)
    : name(name)
    , type(type)
    , semantic(semantic) {

}

bool MicroShaderLoader::DeclWithSemantic::operator <(const DeclWithSemantic& other) const {
    return (std::tie(name, type, semantic) < std::tie(other.name, other.type, other.semantic));
}

void MicroShaderLoader::DeclWithSemantic::JoinUniq(std::vector<DeclWithSemantic>& arr, std::string& out, bool removeDuplicates) {
    if (arr.empty()) {
        return;
    }
    std::sort(arr.begin(), arr.end());

    auto last = arr.begin();
    out.append(fmt::format("{} {} : {};\n", last->type, last->name, last->semantic));

    for(auto it=arr.begin(); it!=arr.end(); ++it) {
        if (it->name != last->name) {
            out.append(fmt::format("{} {} : {};\n", it->type, it->name, it->semantic));
            if (removeDuplicates) {
                last++;
                if (last != it) {
                    std::swap(*last, *it);
                }
            } else {
                last = it;
            }
        } else if (it->type != last->type) {
            throw EngineError("different types for one name '{}': '{}' and '{}'", it->name, it->type, last->type);
        } else if (it->semantic != last->semantic) {
            throw EngineError("different semantics for one name '{}': '{}' and '{}'", it->name, it->semantic, last->semantic);
        }
    }

    if (removeDuplicates) {
        arr.resize(std::distance(arr.begin(), last) + 1);
    }
}

void MicroShaderLoader::PixelData::Append(const PixelData* other, std::vector<std::string>& entrypoints) {
    entrypoints.push_back(other->entrypoint);
    includes.insert(includes.cend(), other->includes.cbegin(), other->includes.cend());
    textures2D.insert(textures2D.cend(), other->textures2D.cbegin(), other->textures2D.cend());
    psInput.insert(psInput.cend(), other->psInput.cbegin(), other->psInput.cend());
    psOutput.insert(psOutput.cend(), other->psOutput.cbegin(), other->psOutput.cend());
    psLocal.insert(psLocal.cend(), other->psLocal.cbegin(), other->psLocal.cend());
    cbuffers.insert(cbuffers.cend(), other->cbuffers.cbegin(), other->cbuffers.cend());
    source.append(other->source + "\n");
}

std::string MicroShaderLoader::PixelData::Generate(const std::vector<std::string>& entrypoints, const MaterialBuilderDesc& desc) {
    std::string res;

    JoinUniq(includes, res);
    Decl::JoinUniq(psOutput, res);
    DeclWithSemantic::JoinUniq(psInput, res, true);
    Decl::JoinUniq(psLocal, res);
    Decl::JoinUniq(cbuffers, res, [&desc](const Decl& d) {
        return fmt::format("cbuffer {} {{ {} {}; }};\n", desc.cbufferNameGenerator(d.name), d.type, d.name);
    });
    JoinUniq(textures2D, res);
    res.append(source);

    res.append("void main(in PSInput psIn, PSOutput psOut) {\nPSLocal psLocal;\n");
    for (const auto& ep: entrypoints) {
        res.append(ep + "(psIn, psLocal, psOut);\n");
    }
    res.append("}");

    return res;
}

std::string MicroShaderLoader::GeometryData::Generate(const std::vector<DeclWithSemantic>& psInput) {
    return std::string();
}

namespace {
    struct ParserDeleter {
        void operator() (ucl_parser *obj) {
            ucl_parser_free(obj);
        }
    };

    using ParserPtr = std::unique_ptr<ucl_parser, ParserDeleter>;

    struct UclDeleter {
        void operator() (ucl_object_t *obj) {
            ucl_object_unref(obj);
        }
    };
}

void MicroShaderLoader::Load(const MaterialBuilderDesc& desc) {
    m_desc = desc;
    m_root = Microshader();
    m_microshaders.clear();
    m_microshaderIDs.clear();

    if (!std::filesystem::is_regular_file(m_desc.shadersSchemaPath)) {
        throw EngineError("failed load microshader files, schema path {} not found", m_desc.shadersSchemaPath.c_str());
    }

    if (!std::filesystem::is_directory(m_desc.shadersDir)) {
        throw EngineError("failed load microshader files, load path {} is not a directory", m_desc.shadersDir.c_str());
    }

    auto requiredExtension = m_desc.shaderFilesExtension;
    if (requiredExtension.empty() || (requiredExtension[0] != '.')) {
        requiredExtension = "." + requiredExtension;
    }

    ParserPtr parser;
    std::unique_ptr<ucl_object_t, UclDeleter> schema;

    parser.reset(ucl_parser_new(UCL_PARSER_DEFAULT));
    if (ucl_parser_add_file(parser.get(), m_desc.shadersSchemaPath.c_str())) {
        schema.reset(ucl_parser_get_object(parser.get()));
    } else if(const char *error = ucl_parser_get_error(parser.get()); error != nullptr) {
        throw EngineError("failed open microshader schema file {}, error: {}", m_desc.shadersSchemaPath.c_str(), error);
    }

    if (!schema) {
        throw EngineError("failed open microshader schema file {}, error: unknown", m_desc.shadersSchemaPath.c_str());
    }

    // groupName => groupID
    std::map<std::string, uint32_t> groupIDs;
    // entrypoint => groupName
    std::map<std::string, std::string> psEntrypoints;
    std::set<std::string> vsEntrypoints;
    // order => groupName
    std::map<int64_t, std::string> psOrder;
    // order => groupName
    std::map<int64_t, std::string> vsOrder;

    for(auto& it: std::filesystem::directory_iterator(m_desc.shadersDir)) {
        ucl::Ucl root;
        const auto path = it.path();
        if (!ReadMicroshader(path, requiredExtension, schema.get(), root)) {
            continue;
        }

        Microshader ms;
        try {
            ParseMicroshader(root, ms);
        } catch(const std::exception& e) {
            throw EngineError("failed parse microshader file {}, error: {}", path.c_str(), e.what());
        }

        if (!ms.ps.isEmpty) {
            const auto itEp = psEntrypoints.find(ms.ps.entrypoint);
            if (itEp == psEntrypoints.cend()) {
                psEntrypoints[ms.ps.entrypoint] = ms.group;
            } else if (itEp->second != ms.group) {
                throw EngineError("name of the pixel microshader entrypoint ({}) is duplicated", ms.ps.entrypoint);
            }

            const auto itOrder = psOrder.find(ms.ps.order);
            if (itOrder == psOrder.cend()) {
                psOrder[ms.ps.order] = ms.group;
            } else if (itOrder->second != ms.group) {
                throw EngineError("value of the pixel microshader order ({}) is duplicated", ms.ps.order);
            }
        }

        for (const auto& [_, vs]: ms.vs) {
            if (vsEntrypoints.find(vs.entrypoint) == vsEntrypoints.cend()) {
                vsEntrypoints.insert(vs.entrypoint);
            } else {
                throw EngineError("name of the vertex microshader entrypoint ({}) is duplicated", vs.entrypoint);
            }

            const auto it = vsOrder.find(vs.order);
            if (it == vsOrder.cend()) {
                vsOrder[vs.order] = ms.group;
            } else if (it->second != ms.group) {
                throw EngineError("value of the vertex microshader order ({}) is duplicated", vs.order);
            }
        }

        if (ms.isRoot) {
            if (!m_root.isEmpty) {
                throw EngineError("found second root shader, curren has name {}, previous - {}", ms.name, m_root.name);
            } else {
                m_root = ms;
                continue;
            }
        }

        auto groupIt = groupIDs.find(ms.group);
        if (groupIt == groupIDs.cend()) {
            ms.groupID = groupIDs.size();
            groupIDs[ms.group] = ms.groupID;
        } else {
            ms.groupID = groupIt->second;
        }

        auto id = static_cast<uint32_t>(m_microshaders.size());
        if (id >= 64) {
            throw EngineError("microshaders type number are over the limit (64)");
        }
        if (m_microshaderIDs.find(ms.name) != m_microshaderIDs.cend()) {
            throw EngineError("name of the microshader ({}) is duplicated", ms.name);
        }
        m_microshaders.push_back(ms);
        m_microshaderIDs[ms.name] = id;
    }

    if (m_root.isEmpty) {
        throw EngineError("failed load microshader files, root microshader not found");
    }
}

uint64_t MicroShaderLoader::GetMask(const std::string& name) const {
    auto it = m_microshaderIDs.find(name);
    if (it == m_microshaderIDs.cend()) {
        throw EngineError("not found microshader with name {}", name);
    }

    return 1 << it->second;
}

MicroShaderLoader::Source MicroShaderLoader::GetSources(uint64_t mask) const {
    Source src;

    GeometryData gs;
    bool psIsOverride = false;
    std::vector<const PixelData*> psArr;
    std::vector<const Microshader*> msArr;
    bool groups[sizeof(decltype(mask)) << 3] = {false};

    if (!m_root.ps.isEmpty) {
        psArr.push_back(&m_root.ps);
    }
    msArr.push_back(&m_root);
    for (uint64_t id=0; id!=64; ++id) {
        if ((mask & (uint64_t(1) << id)) != 0) {
           if (id >= m_microshaders.size()) {
               throw EngineError("invalid microshaders mask {} for get sources, id = {} not exists", mask, id);
            }
            const auto& ms = m_microshaders[id];
            if (groups[ms.groupID]) {
                throw EngineError("invalid microshaders mask {} for get sources, group {} is duplicated", mask, id);
            }
            groups[ms.groupID] = true;

            if (!ms.ps.isEmpty) {
                if (ms.ps.isOverride && psIsOverride) {
                    throw EngineError("invalid microshaders mask {} for get sources, flag 'isOverride' for pixel shader is duplicated", mask, id);
                }
                if (ms.ps.isOverride) {
                    psArr = {&ms.ps};
                    psIsOverride = true;
                } else if (!psIsOverride) {
                    psArr.push_back(&ms.ps);
                }
            }
            if (!ms.gs.isEmpty) {
                if (!gs.isEmpty) {
                    throw EngineError("invalid microshaders mask {} for get sources, found double geometry shader", mask);
                }
                gs = ms.gs;
            }

            if (!src.name.empty()) {
                src.name += ".";
            }
            src.name += ms.name;
            msArr.push_back(&ms);
        }
    }

    if (psArr.empty()) {
        throw EngineError("invalid microshaders mask {} for get sources, not found pixel microshaders", mask);
    }
    std::sort(psArr.begin(), psArr.end(), [](const PixelData* a, const PixelData* b) -> bool { return a->order > b->order; });
    PixelData psBase;
    std::vector<std::string> entrypoints;
    entrypoints.reserve(psArr.size());
    for (const auto* ps: psArr) {
        psBase.Append(ps, entrypoints);
    }

    try {
        src.ps = psBase.Generate(entrypoints, m_desc);
    } catch(const std::exception& e) {
        throw EngineError("invalid microshaders mask {} for get sources, can't generate pixel shader: {}", e.what());
    }

    if (!gs.isEmpty) {
        try {
            src.gs = gs.Generate(psBase.psInput);
        } catch(const std::exception& e) {
            throw EngineError("invalid microshaders mask {} for get sources, can't generate geometry shader: {}", e.what());
        }
    }

    return src;
}


bool MicroShaderLoader::ReadMicroshader(const std::filesystem::path& filepath, const std::string& requiredExtension, ucl_object_t* schema, ucl::Ucl& section) {
    if (!std::filesystem::is_regular_file(filepath)) {
        return false;
    }

    if (filepath.extension() != requiredExtension) {
        return false;
    }

    ucl_object_t* rootRaw = nullptr;
    auto parser = ParserPtr(ucl_parser_new(UCL_PARSER_DEFAULT));
    if (ucl_parser_add_file(parser.get(), filepath.c_str())) {
        rootRaw = ucl_parser_get_object(parser.get());
    } else if(const char *error = ucl_parser_get_error(parser.get()); error != nullptr) {
        throw EngineError("failed open microshader file {}, error: {}", filepath.c_str(), error);
    }

    section = ucl::Ucl(rootRaw);
    if (!section) {
        throw EngineError("failed open microshader file {}, error: unknown", filepath.c_str());
    }

    try {
        ucl_schema_error schemaErr;
        if (!ucl_object_validate(schema, rootRaw, &schemaErr)) {
            throw EngineError("failed validate microshader file {}, key: '{}', value: '{}', error: '{}'",
                filepath.c_str(),
                schemaErr.obj->key,
                ucl_object_emit(schemaErr.obj, UCL_EMIT_CONFIG),
                schemaErr.msg);
        }
    } catch(const std::exception& e) {
        throw EngineError("failed validate microshader file {}, error: {}", filepath.c_str(), e.what());
    }

    return true;
}

void MicroShaderLoader::ParseMicroshader(const ucl::Ucl& section, Microshader& ms) {
    ms.isEmpty = false;
    for (const auto &it: section) {
        if (it.key() == "name") {
            ms.name = it.string_value();
        } else if (it.key() == "group") {
            ms.group = it.string_value();
        } else if (it.key() == "root") {
            ms.isRoot = it.bool_value();
        } else if (it.key() == "vertex") {
            ParseVertex(it, it.key(), ms.vs);
        } else if (it.key() == "pixel") {
            ParsePixel(it, it.key(), ms.ps);
        } else if (it.key() == "geometry") {
            ParseGeometry(it, it.key(), ms.gs);
        } else {
            throw EngineError("unknown section: {} with data: {}", it.key(), it.dump());
        }
    }
}

void MicroShaderLoader::ParseVertex(const ucl::Ucl& section, const std::string& sectionName, std::map<std::string, VertexData>& data) {
    for (const auto &it: section) {
        VertexData vs;
        ParseVertexItem(it, sectionName + "." + it.key(), vs);
        data[it.key()] = vs;
    }
}

void MicroShaderLoader::ParseVertexItem(const ucl::Ucl& section, const std::string& sectionName, VertexData& data) {
    data.isEmpty = false;
    for (const auto &it: section) {
        if (it.key() == "entrypoint") {
            data.entrypoint = it.string_value();
            if (data.entrypoint == "main") {
                throw EngineError("entrypoints with the name 'main' is disabled");
            }
        } else if (it.key() == "order") {
            data.order = it.int_value();
        } else if (it.key() == "override") {
            data.isOverride = it.bool_value();
        } else if (it.key() == "include") {
            ParseArray(it, data.includes);
        } else if (it.key() == "VSInput") {
            ParseArray(it, data.vsInput);
        } else if (it.key() == "cbuffers") {
            ParseDecl(it, data.cbuffers);
        } else if (it.key() == "textures2D") {
            ParseArray(it, data.textures2D);
        } else if (it.key() == "source") {
            data.source = it.string_value();
        } else {
            throw EngineError("unknown section: {}.{} with data: {}", sectionName, it.key(), it.dump());
        }
    }
}

void MicroShaderLoader::ParsePixel(const ucl::Ucl& section, const std::string& sectionName, PixelData& data) {
    data.isEmpty = false;
    for (const auto &it: section) {
        if (it.key() == "entrypoint") {
            data.entrypoint = it.string_value();
            if (data.entrypoint == "main") {
                throw EngineError("entrypoints with the name 'main' is disabled");
            }
        } else if (it.key() == "order") {
            data.order = it.int_value();
        } else if (it.key() == "override") {
            data.isOverride = it.bool_value();
        } else if (it.key() == "include") {
            ParseArray(it, data.includes);
        } else if (it.key() == "PSOutput") {
            ParseDecl(it, data.psOutput);
        } else if (it.key() == "PSInput") {
            ParseDeclWithSemantic(it, data.psInput);
        } else if (it.key() == "PSLocal") {
            ParseDecl(it, data.psLocal);
        } else if (it.key() == "cbuffers") {
            ParseDecl(it, data.cbuffers);
        } else if (it.key() == "textures2D") {
            ParseArray(it, data.textures2D);
        } else if (it.key() == "source") {
            data.source = it.string_value();
        } else {
            throw EngineError("unknown section: {}.{} with data: {}", sectionName, it.key(), it.dump());
        }
    }
}

void MicroShaderLoader::ParseGeometry(const ucl::Ucl& section, const std::string& sectionName, GeometryData& data) {
    data.isEmpty = false;
    for (const auto &it: section) {
        if (it.key() == "include") {
            ParseArray(it, data.includes);
        } else if (it.key() == "GSOutput") {
            ParseArray(it, data.gsOutput);
        } else if (it.key() == "GSInput") {
            ParseArray(it, data.gsInput);
        } else if (it.key() == "cbuffers") {
            ParseDecl(it, data.cbuffers);
        } else if (it.key() == "textures2D") {
            ParseArray(it, data.textures2D);
        } else if (it.key() == "source") {
            data.source = it.string_value();
        } else {
            throw EngineError("unknown section: {}.{} with data: {}", sectionName, it.key(), it.dump());
        }
    }
}

void MicroShaderLoader::ParseArray(const ucl::Ucl& section, std::vector<std::string>& data) {
    for (const auto& it: section) {
        data.push_back(it.string_value());
    }
}

void MicroShaderLoader::ParseDecl(const ucl::Ucl& section, std::vector<Decl>& data) {
    for (const auto& it: section) {
        data.emplace_back(it.key(), it.string_value());
    }
}

void MicroShaderLoader::ParseDeclWithSemantic(const ucl::Ucl& section, std::vector<DeclWithSemantic>& data) {
    for (const auto& it: section) {
        data.emplace_back(it.key(), it[0].string_value(), it[1].string_value());
    }
}
