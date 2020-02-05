#include "core/material/micro_shader_loader.h"

#include "core/common/exception.h"


// void MicroShaderLoader::ShaderData::Append(const MicroShaderLoader::ShaderData& other) {
//     if ((other.mixing == Mixing::Replace) && (mixing == Mixing::Replace)) {
//         throw EngineError("double 'mixing' value in microshaders");
//     }

//     if ((other.mixing == Mixing::Replace) && (mixing == Mixing::Add)) {
//         includes.clear();
//         textures2D.clear();
//         cbuffers.clear();
//         inputs.clear();
//         mixing = Mixing::Replace;
//         source.clear();
//     } else if ((other.mixing == Mixing::Add) && (mixing == Mixing::Replace)) {
//         return;
//     } else if ((other.mixing != Mixing::Add) || (mixing != Mixing::Add)) {
//         throw EngineError("unknown 'mixing' value for microshader: {}", static_cast<uint8_t>(other.mixing));
//     }

//     includes.insert(other.includes.begin(), other.includes.end());
//     textures2D.insert(other.textures2D.begin(), other.textures2D.end());
//     cbuffers.insert(other.cbuffers.begin(), other.cbuffers.end());

//     for (const auto& [name, itype]: other.inputs) {
//         const auto it = inputs.find(name);
//         if (it != inputs.cend()) {
//             if (it->second.type != itype.type) {
//                 throw EngineError("different types for one input key '{}': '{}' and '{}'",
//                     name, it->second.type, itype.type);
//             }
//             if (it->second.semantic != itype.semantic) {
//                 throw EngineError("different semantics for one input key '{}': '{}' and '{}'",
//                     name, it->second.semantic, itype.semantic);
//             }
//         } else {
//             inputs[name] = itype;
//         }
//     }

//     source += other.source;
// }

// std::string MicroShaderLoader::ShaderData::GenParametersToStr(const MaterialBuilderDesc& desc) {
//     std::string s;
//     for (const auto& file: includes) {
//         s += "#include \"" + file + "\"\n";
//     }
//     for (const auto& t: textures2D) {
//         s += fmt::format("Texture2D {0};\nSamplerState {0}{1};\n", t, desc.samplerSuffix);
//     }
//     for (const auto& cb: cbuffers) {
//         s += desc.cbufferGenerator(cb) + "\n";
//     }
//     if (!inputs.empty()) {
//         s += fmt::format("struct {} {{\n", desc.psInputStructName);
//         for (const auto& [key, inputType]: inputs) {
//             s += fmt::format("{} {}: {};\n", inputType.type, key, inputType.semantic);
//         }
//         s += "};\n";
//     }

//     return s;
// }

MicroShaderLoader::Decl::Decl(const std::string& name, const std::string& type)
    : name(name)
    , type(type) {

}

MicroShaderLoader::DeclWithSemantic::DeclWithSemantic(const std::string& name, const std::string& type, const std::string& semantic)
    : name(name)
    , type(type)
    , semantic(semantic) {

}

void MicroShaderLoader::PixelData::Append(const PixelData& other) {
    if (other.isEmpty) {
        return;
    }

    includes.insert(includes.cend(), other.includes.cbegin(), other.includes.cend());
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

    bool groups[sizeof(decltype(mask)) << 3] = {false};
    std::vector<Microshader> microshaders;
    PixelData ps;
    microshaders.push_back(m_root);
    for (uint64_t id=0; id!=64; ++id) {
        if ((mask & (uint64_t(1) << id)) != 0) {
           if (id >= m_microshaders.size()) {
               throw EngineError("invalid microshaders mask for get sources, id = {} not exists", id);
            }
            const auto& ms = m_microshaders[id];
            if (groups[ms.groupID]) {
                throw EngineError("invalid microshaders mask for get sources, group {} is duplicated", id);
            }
            groups[ms.groupID] = true;

            if (!src.name.empty()) {
                src.name += ".";
            }
            src.name += ms.name;
            microshaders.push_back(ms);
            ps.Append(ms.ps);
        }
    }

//     ShaderData vs, ps, gs;
//     for (const auto& ms: microshaders) {
//         vs.Append(ms.vs);
//         ps.Append(ms.ps);
//         gs.Append(ms.gs);
//     }

//     src.vs = vs.GenParametersToStr(m_desc) + vs.source;
//     src.ps = ps.GenParametersToStr(m_desc) + ps.source;
//     src.gs = gs.GenParametersToStr(m_desc) + gs.source;

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
