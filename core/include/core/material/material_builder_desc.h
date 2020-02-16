#pragma once

#include <string>
#include <functional>
#include <filesystem>


struct MaterialBuilderDesc {
    std::string samplerSuffix = "Sampler";
    std::function<std::string (const std::string&)> cbufferNameGenerator;

    std::filesystem::path shadersDir;
    std::filesystem::path shadersSchemaPath;
    std::string shaderFilesExtension;
};
