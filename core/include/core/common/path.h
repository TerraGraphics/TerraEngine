#pragma once

#include <map>
#include <filesystem>
#include "core/common/ctor.h"


class FileManager : Fixed {
public:
    FileManager() = default;
    ~FileManager() = default;

    void AddRootAlias(const std::string& alias, const std::filesystem::path& path);

    std::filesystem::path CurrentPath() const;

    bool GetRealPath(const std::filesystem::path& inPath, std::filesystem::path& outPath, std::string& error) const noexcept;
    bool GetRealPath(const std::filesystem::path& inPath, std::filesystem::path& outPath) const noexcept;
    std::filesystem::path GetRealPath(const std::filesystem::path& inPath) const;

    bool ReadFullFile(const std::filesystem::path& path, std::string& data, std::string& error) const noexcept;
    bool ReadFullFile(const std::filesystem::path& path, std::string& data) const noexcept;
    std::string ReadFullFile(const std::filesystem::path& path) const;

private:
    std::map<std::string, std::filesystem::path> m_aliases;
};
