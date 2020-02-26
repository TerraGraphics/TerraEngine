#include "core/material/microshader_loader.h"

#include <set>
#include <memory>
#include <utility>
#include <type_traits>

#include "core/dg/ucl.h"
#include "core/common/exception.h"


struct ucl_parser;
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

void MicroshaderLoader::Load(const MaterialBuilderDesc& desc) {
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
            ms.groupID = static_cast<uint32_t>(groupIDs.size());
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

uint64_t MicroshaderLoader::GetMask(const std::string& name) const {
    auto it = m_microshaderIDs.find(name);
    if (it == m_microshaderIDs.cend()) {
        throw EngineError("not found microshader with name '{}'", name);
    }

    return 1 << it->second;
}

MicroshaderLoader::Source MicroshaderLoader::GetSources(uint64_t mask, const msh::SemanticDecls& vertexInput) const {
    if (const auto it=m_cache.find(mask); it != m_cache.cend()) {
        return it->second;
    }

    Source src;
    msh::PixelShader ps;
    msh::VertexShader vs;
    msh::GeometryShader gs;
    bool groups[sizeof(decltype(mask)) << 3] = {false};

    try {
        ps.Append(&m_root.ps);
        vs.Append(m_root.vs);
        gs.Append(m_root.gs);
        for (uint64_t id=0; id!=64; ++id) {
            if ((mask & (uint64_t(1) << id)) == 0) {
                continue;
            }
            if (id >= m_microshaders.size()) {
                throw EngineError("id = {} not exists", id);
            }
            const auto& ms = m_microshaders[id];
            if (groups[ms.groupID]) {
                throw EngineError("group {} is duplicated", id);
            }
            groups[ms.groupID] = true;

            ps.Append(&ms.ps);
            vs.Append(ms.vs);
            gs.Append(ms.gs);

            if (!src.name.empty()) {
                src.name += ".";
            }
            src.name += ms.name;
        }
    } catch(const std::exception& e) {
        throw EngineError("invalid microshaders with mask {} for get sources, {}", mask, e.what());
    }

    try {
        ps.Generate(m_desc, src.ps);
        if (gs.IsEmpty()) {
            vs.Generate(m_desc, vertexInput, ps.GetInput(), src.vs);
        } else {
            gs.Generate(m_desc, ps.GetInput(), src.gs);
            vs.Generate(m_desc, vertexInput, gs.GetInput(), src.vs);
        }
        src.gsOutputNumber = ps.GetOutputNumber();
    } catch(const std::exception& e) {
        throw EngineError("invalid microshaders '{}' (mask {}) for get sources, {}", src.name, mask, e.what());
    }

    m_cache[mask] = src;
    return src;
}

bool MicroshaderLoader::ReadMicroshader(const std::filesystem::path& filepath, const std::string& requiredExtension, ucl_object_t* schema, ucl::Ucl& section) {
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

void MicroshaderLoader::ParseMicroshader(const ucl::Ucl& section, Microshader& ms) {
    ms.isEmpty = false;
    for (const auto &it: section) {
        if (it.key() == "name") {
            ms.name = it.string_value();
        } else if (it.key() == "group") {
            ms.group = it.string_value();
        } else if (it.key() == "root") {
            ms.isRoot = it.bool_value();
        } else if (it.key() == "vertex") {
            for (const auto &itVertex: it) {
                msh::VertexMicroshader vs;
                vs.Parse(itVertex, it.key());
                ms.vs[itVertex.key()] = vs;
            }
        } else if (it.key() == "pixel") {
            ms.ps.Parse(it);
        } else if (it.key() == "geometry") {
            ms.gs.Parse(it);
        } else {
            throw EngineError("unknown section: {} with data: {}", it.key(), it.dump());
        }
    }
}
