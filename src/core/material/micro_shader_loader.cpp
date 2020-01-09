#include "core/material/micro_shader_loader.h"

#include "core/common/exception.h"


void MicroShaderLoader::Load(const std::filesystem::path& dirPath, const std::string& extension) {
    if (!std::filesystem::is_directory(dirPath)) {
        throw EngineError("failed load microshader files, load path {} is not a directory", dirPath.c_str());
    }

    auto fullExtension = extension;
    if (fullExtension.empty() || (fullExtension[0] != '.')) {
        fullExtension = "." + fullExtension;
    }

    for(auto& it: std::filesystem::directory_iterator(dirPath)) {
        const auto path = it.path();
        if (!std::filesystem::is_regular_file(it.path())) {
            continue;
        }
        if (path.extension() != fullExtension) {
            continue;
        }

        std::string err;
        auto root = ucl::Ucl::parse_from_file(path.c_str(), err);
        if (!err.empty()) {
            throw EngineError("failed open microshader file {}, error: {}", path.c_str(), err);
        }
        if (!root) {
            throw EngineError("failed open microshader file {}, error: unknown", path.c_str());
        }

        try {
            ParseMicroshader(root);
        } catch(const std::exception& e) {
            throw EngineError("failed parse microshader file {}, error: {}", path.c_str(), e.what());
        }
    }

    for (const auto& sh: m_defaultMicroShaders) {
        if (sh.isEmpty) {
            throw EngineError("not found default microshader for group {}", sh.group);
        }
    }
}

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
            ParseShader(it, it.key(), ms.vs);
        } else if (it.key() == "pixel") {
            ParseShader(it, it.key(), ms.ps);
        } else if (it.key() == "geometry") {
            ParseShader(it, it.key(), ms.gs);
        } else {
            throw EngineError("unknown section: {} with data: {}", it.key(), it.dump());
        }
    }

    if (isRoot) {
        m_root = ms;
        return;
    }

    if (ms.group.empty()) {
        throw EngineError("required section 'meta::group' was not found");
    }
    auto groupIt = m_groupIDs.find(ms.group);
    if (groupIt == m_groupIDs.cend()) {
        ms.groupID = m_groupIDs.size();
        m_groupIDs[ms.group] = ms.groupID;
        Microshader def;
        def.isEmpty = true;
        def.groupID = ms.groupID;
        def.group = ms.group;
        m_defaultMicroShaders.push_back(def);
    } else {
        ms.groupID = groupIt->second;
    }

    if (!ms.name.empty()) {
        // named microshader
        auto id = static_cast<uint32_t>(m_namedMicroShaders.size());
        if (id >= 64) {
            throw EngineError("microshaders type number are over the limit (64)");
        }
        if (m_namedMicroShaderIDs.find(ms.name) != m_namedMicroShaderIDs.cend()) {
            throw EngineError("name of the microshader ({}) is duplicated", ms.name);
        }
        m_namedMicroShaders.push_back(ms);
        m_namedMicroShaderIDs[ms.name] = id;
    } else {
        if (!m_defaultMicroShaders[ms.groupID].isEmpty) {
            throw EngineError("default microshader for group {} already exists", ms.group);
        }
        m_defaultMicroShaders[ms.groupID] = ms;
    }
}

void MicroShaderLoader::ParseShader(const ucl::Ucl& section, const std::string& sectionName, ShaderData& shader) {
    for (const auto &it: section) {
        if (it.key() == "parameters") {
            ParseParameters(it, sectionName + ".parameters", shader);
        } else if (it.key() == "source") {
            shader.source = it.string_value();
        } else {
            throw EngineError("unknown section: {}.{} with data: {}", sectionName, it.key(), it.dump());
        }
    }
}

void MicroShaderLoader::ParseParameters(const ucl::Ucl& section, const std::string& sectionName, ShaderData& shader) {
    for (const auto &it: section) {
        if (it.key() == "textures2D") {
            for (const auto& texIt: it) {
                shader.textures2D.insert(texIt.string_value());
            }
        } else if (it.key() == "cbuffers") {
            for (const auto& nameIt: it) {
                shader.cbuffers.insert(nameIt.string_value());
            }
        } else {
            throw EngineError("unknown section: {}.{} with data: {}", sectionName, it.key(), it.dump());
        }
    }
}
