#pragma once

#include <string>
#include <functional>
#include <filesystem>
#include "core/material/vertex_decl.h"


struct MaterialBuilderDesc {
    std::string samplerSuffix = "Sampler";
    std::string psInputStructName = "PSInput";
    std::function<std::string (const std::string&)> cbufferGeneratorOld;
    std::function<std::string (const std::string&)> cbufferNameGenerator;

    std::filesystem::path shadersDirOld;
    std::filesystem::path shadersDir;
    std::filesystem::path shadersSchemaPath;
    std::string shaderFilesExtension;

    VertexDecl additionalVertexDecl;
};
