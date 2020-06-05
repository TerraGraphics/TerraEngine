#include "core/path/path.h"

#include <cstring>
#include <numeric>
#include <fstream>
#include <errno.h>
#include <utility>
#include <iterator>
#include <functional>

#include "fmt/fmt.h"
#include "core/common/exception.h"


void FileManager::AddRootAlias(const std::string& alias, const std::filesystem::path& path) {
    m_aliases[alias] = path;
}

std::filesystem::path FileManager::CurrentPath() const {
    return std::filesystem::current_path();
}

bool FileManager::GetRealPath(const std::filesystem::path& inPath, std::filesystem::path& outPath, std::string& error) const noexcept {
    if (inPath.empty()) {
        error = fmt::format("not found real path for empty path");
        return false;
    }

    if (std::filesystem::exists(inPath)) {
        outPath = std::filesystem::canonical(inPath);
        return true;
    }

    const auto firstPathElement = inPath.begin();
    const auto it = m_aliases.find(*firstPathElement);
    if (it != m_aliases.cend()) {
        outPath = std::filesystem::canonical(std::accumulate(std::next(firstPathElement), inPath.end(), it->second, std::divides{}));
        if (std::filesystem::exists(outPath)) {
            return true;
        }
    }

    error = fmt::format("not found real path for: {}", inPath.c_str());
    return false;
}

bool FileManager::GetRealPath(const std::filesystem::path& inPath, std::filesystem::path& outPath) const noexcept {
    std::string error;
    return GetRealPath(inPath, outPath, error);
}

std::filesystem::path FileManager::GetRealPath(const std::filesystem::path& inPath) const {
    std::string error;
    std::filesystem::path result;
    if (!GetRealPath(inPath, result, error)) {
        throw EngineError(error);
    }

    return result;
}

bool FileManager::ReadFullFile(const std::filesystem::path& path, std::string& data, std::string& error) const noexcept {
    std::filesystem::path fullPath;
    if (!GetRealPath(path, fullPath, error)) {
        return false;
    }

    std::ifstream ifs(fullPath.c_str(), std::ifstream::in);
    if(!ifs) {
        error = fmt::format("couldn't open file '{}', error: {}", fullPath.c_str(), strerror(errno));
        return false;
    }

    ifs.seekg(0, std::ios::end);
    data.reserve(static_cast<size_t>(ifs.tellg()));
    ifs.seekg(0, std::ios::beg);

    data.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();

    return true;
}

bool FileManager::ReadFullFile(const std::filesystem::path& path, std::string& data) const noexcept {
    std::string error;
    return ReadFullFile(path, data, error);
}

std::string FileManager::ReadFullFile(const std::filesystem::path& path) const {
    std::string error;
    std::string result;
    if (!ReadFullFile(path, result, error)) {
        throw EngineError(error);
    }

    return result;
}
