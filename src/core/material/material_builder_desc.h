#pragma once

#include <string>
#include <functional>
#include <filesystem>


struct MaterialBuilderDesc {
    std::string samplerSuffix = "Sampler";
    std::string psInputStructName = "PSInput";
    std::function<std::string (const std::string&)> cbufferGenerator;

    std::filesystem::path shadersDirOld;
    std::filesystem::path shadersDir;
    std::filesystem::path shadersSchemaPath;
    std::string shaderFilesExtension;
};
