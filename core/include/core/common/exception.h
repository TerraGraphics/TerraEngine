#pragma once

#include <stdexcept>
#include <fmt/format.h>


class EngineError : public std::runtime_error {
public:
    explicit EngineError(const std::string& msg)
        : std::runtime_error(msg) {}
    template <typename S, typename... Args> EngineError(const S& format_str, Args&&... args)
        : std::runtime_error(fmt::format(format_str, args...)) {}
};
