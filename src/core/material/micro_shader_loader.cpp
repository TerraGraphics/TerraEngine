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
    // m_namedMicroShaders.clear();
    // m_namedMicroShaderIDs.clear();
    // m_defaultMicroShaders.clear();
    // m_groupIDs.clear();

    if (!std::filesystem::is_regular_file(m_desc.shadersSchemaPath)) {
        throw EngineError("failed load microshader files, schema path {} not found", m_desc.shadersSchemaPath.c_str());
    }

    if (!std::filesystem::is_directory(m_desc.shadersDir)) {
        throw EngineError("failed load microshader files, load path {} is not a directory", m_desc.shadersDir.c_str());
    }

    auto fullExtension = m_desc.shaderFilesExtension;
    if (fullExtension.empty() || (fullExtension[0] != '.')) {
        fullExtension = "." + fullExtension;
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

    for(auto& it: std::filesystem::directory_iterator(m_desc.shadersDir)) {
        const auto path = it.path();
        if (!std::filesystem::is_regular_file(it.path())) {
            continue;
        }
        if (path.extension() != fullExtension) {
            continue;
        }

        ucl_object_t* rootRaw = nullptr;
        parser.reset(ucl_parser_new(UCL_PARSER_DEFAULT));
        if (ucl_parser_add_file(parser.get(), path.c_str())) {
            rootRaw = ucl_parser_get_object(parser.get());
        } else if(const char *error = ucl_parser_get_error(parser.get()); error != nullptr) {
            throw EngineError("failed open microshader file {}, error: {}", path.c_str(), error);
        }

        auto root = ucl::Ucl(rootRaw);
        if (!root) {
            throw EngineError("failed open microshader file {}, error: unknown", path.c_str());
        }

        try {
            ucl_schema_error schemaErr;
            if (!ucl_object_validate(schema.get(), rootRaw, &schemaErr)) {
                throw EngineError("failed validate microshader file {}, key: '{}', value: '{}', error: '{}'",
                    path.c_str(),
                    schemaErr.obj->key,
                    ucl_object_emit(schemaErr.obj, UCL_EMIT_CONFIG),
                    schemaErr.msg);
            }
        } catch(const std::exception& e) {
            throw EngineError("failed validate microshader file {}, error: {}", path.c_str(), e.what());
        }

        try {
            ParseMicroshader(root);
        } catch(const std::exception& e) {
            throw EngineError("failed parse microshader file {}, error: {}", path.c_str(), e.what());
        }
    }

    // for (const auto& sh: m_defaultMicroShaders) {
    //     if (sh.isEmpty) {
    //         throw EngineError("not found default microshader for group {}", sh.group);
    //     }
    // }
}

// uint64_t MicroShaderLoader::GetMask(const std::string& name) const {
//     auto it = m_namedMicroShaderIDs.find(name);
//     if (it == m_namedMicroShaderIDs.cend()) {
//         throw EngineError("not found microshader with name {}", name);
//     }

//     return 1 << it->second;
// }

// MicroShaderLoader::Source MicroShaderLoader::GetSources(uint64_t mask) const {
//     Source src;

//     std::vector<Microshader> microshaders(m_defaultMicroShaders);
//     microshaders.push_back(m_root);
//     for (uint64_t id=0; id!=64; ++id) {
//         if ((mask & (uint64_t(1) << id)) != 0) {
//             if (id >= m_namedMicroShaders.size()) {
//                 throw EngineError("invalid microshaders mask for get sources, id = {} not exists", id);
//             }
//             const auto& ms = m_namedMicroShaders[id];
//             if (!src.name.empty()) {
//                 src.name += ".";
//             }
//             src.name += ms.name;
//             microshaders[ms.groupID] = ms;
//         }
//     }

//     ShaderData vs, ps, gs;
//     for (const auto& ms: microshaders) {
//         vs.Append(ms.vs);
//         ps.Append(ms.ps);
//         gs.Append(ms.gs);
//     }

//     src.vs = vs.GenParametersToStr(m_desc) + vs.source;
//     src.ps = ps.GenParametersToStr(m_desc) + ps.source;
//     src.gs = gs.GenParametersToStr(m_desc) + gs.source;

//     return src;
// }

void MicroShaderLoader::ParseMicroshader(const ucl::Ucl& section) {
    Microshader ms;
    ms.isEmpty = false;
    bool isRoot = false;
    for (const auto &it: section) {
        if (it.key() == "name") {
            ms.name = it.string_value();
        } else if (it.key() == "group") {
            ms.group = it.string_value();
        } else if (it.key() == "root") {
            isRoot = it.bool_value();
        } else if (it.key() == "vertex") {
        //     ParseShader(it, it.key(), ms.vs);
        } else if (it.key() == "pixel") {
            ParsePixel(it, it.key(), ms.ps);
        } else if (it.key() == "geometry") {
            ParseGeometry(it, it.key(), ms.gs);
        } else {
            throw EngineError("unknown section: {} with data: {}", it.key(), it.dump());
        }
    }

    if (isRoot) {
        m_root = ms;
        return;
    }

    // if (ms.group.empty()) {
    //     throw EngineError("required section 'group' was not found");
    // }
    // auto groupIt = m_groupIDs.find(ms.group);
    // if (groupIt == m_groupIDs.cend()) {
    //     ms.groupID = m_groupIDs.size();
    //     m_groupIDs[ms.group] = ms.groupID;
    //     Microshader def;
    //     def.isEmpty = true;
    //     def.groupID = ms.groupID;
    //     def.group = ms.group;
    //     m_defaultMicroShaders.push_back(def);
    // } else {
    //     ms.groupID = groupIt->second;
    // }

    // if (!ms.name.empty()) {
    //     // named microshader
    //     auto id = static_cast<uint32_t>(m_namedMicroShaders.size());
    //     if (id >= 64) {
    //         throw EngineError("microshaders type number are over the limit (64)");
    //     }
    //     if (m_namedMicroShaderIDs.find(ms.name) != m_namedMicroShaderIDs.cend()) {
    //         throw EngineError("name of the microshader ({}) is duplicated", ms.name);
    //     }
    //     if (m_defaultMicroShaders[ms.groupID].isEmpty &&
    //         (ms.vs.isEmpty || (ms.vs.mixing == Mixing::Replace)) &&
    //         (ms.ps.isEmpty || (ms.ps.mixing == Mixing::Replace)) &&
    //         (ms.gs.isEmpty || (ms.gs.mixing == Mixing::Replace))) {
    //         m_defaultMicroShaders[ms.groupID].isEmpty = false;
    //         m_defaultMicroShaders[ms.groupID].autogen = true;
    //     }
    //     m_namedMicroShaders.push_back(ms);
    //     m_namedMicroShaderIDs[ms.name] = id;
    // } else {
    //     if (!m_defaultMicroShaders[ms.groupID].isEmpty && !m_defaultMicroShaders[ms.groupID].autogen) {
    //         throw EngineError("default microshader for group {} already exists", ms.group);
    //     }
    //     m_defaultMicroShaders[ms.groupID] = ms;
    // }
}

void MicroShaderLoader::ParsePixel(const ucl::Ucl& section, const std::string& sectionName, PixelData& data) {
    data.isEmpty = false;
    for (const auto &it: section) {
        if (it.key() == "entrypoint") {
            data.entrypoint = it.string_value();
        } else if (it.key() == "order") {
            data.order = it.int_value();
        } else if (it.key() == "override") {
            data.isOverride = it.bool_value();
        } else if (it.key() == "include") {
            for (const auto& fileIt: it) {
                data.includes.push_back(fileIt.string_value());
            }
        } else if (it.key() == "PSOutput") {
            ParseKV(it, sectionName + ".PSOutput", data.psOutput);
        } else if (it.key() == "PSInput") {
            ParseInputs(it, sectionName + ".PSInput", data.psInput);
        } else if (it.key() == "PSLocal") {
            ParseKV(it, sectionName + ".PSLocal", data.psLocal);
        } else if (it.key() == "cbuffers") {
            ParseKV(it, sectionName + ".cbuffers", data.cbuffers);
        } else if (it.key() == "textures2D") {
            for (const auto& fileIt: it) {
                data.textures2D.push_back(fileIt.string_value());
            }
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
            for (const auto& fileIt: it) {
                data.includes.push_back(fileIt.string_value());
            }
        } else if (it.key() == "GSOutput") {
            for (const auto& fileIt: it) {
                data.gsOutput.push_back(fileIt.string_value());
            }
        } else if (it.key() == "GSInput") {
            for (const auto& fileIt: it) {
                data.gsInput.push_back(fileIt.string_value());
            }
        } else if (it.key() == "cbuffers") {
            ParseKV(it, sectionName + ".cbuffers", data.cbuffers);
        } else if (it.key() == "textures2D") {
            for (const auto& fileIt: it) {
                data.textures2D.push_back(fileIt.string_value());
            }
        } else if (it.key() == "source") {
            data.source = it.string_value();
        } else {
            throw EngineError("unknown section: {}.{} with data: {}", sectionName, it.key(), it.dump());
        }
    }
}

void MicroShaderLoader::ParseKV(const ucl::Ucl& section, const std::string& sectionName, std::map<std::string, std::string>& data) {
    for (const auto& inputIt: section) {
        const auto name = inputIt.key();
        const auto value = inputIt.string_value();
        auto it = data.find(name);
        if (it != data.cend()) {
            if (it->second != value) {
                throw EngineError("different types for one key '{}.{}', current type: {}, previous: {}",
                    sectionName, name, it->second, value);
            }
        } else {
            data[name] = value;
        }
    }
}

void MicroShaderLoader::ParseInputs(const ucl::Ucl& section, const std::string& sectionName, std::map<std::string, InputType>& data) {
    for (const auto& inputIt: section) {
        if (inputIt.size() != 2) {
            throw EngineError("section with types (for '{}.{}') shall be of two elements, in fact it is equal to {}",
                    sectionName, inputIt.key(), inputIt.dump());
        }
        InputType inputType{inputIt[0].string_value(), inputIt[1].string_value()};
        const auto name = inputIt.key();

        auto it = data.find(name);
        if (it != data.cend()) {
            if (it->second.type != inputType.type) {
                throw EngineError("different types for one inputs key '{}.{}', current type: {}, previous: {}",
                    sectionName, inputIt.key(), inputType.type, it->second.type);
            }
            if (it->second.semantic != inputType.semantic) {
                throw EngineError("different semantics for one inputs key '{}.{}', current type: {}, previous: {}",
                    sectionName, inputIt.key(), inputType.semantic, it->second.semantic);
            }
        } else {
            data[name] = inputType;
        }
    }
}
