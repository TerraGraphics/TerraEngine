#pragma once

#include <cstdint>


namespace gs {

enum class PinTypes : uint8_t {
    EMBEDDED = 0,
    INPUT = 1,
    OUTPUT = 2,
};

struct MetaNames {
    static constexpr const char* CLASS = "cls";
    static constexpr const char* TYPE_CLASS = "tcls";
    static constexpr const char* PIN = "pin";
    static constexpr const char* PIN_TYPE = "ptype";
    static constexpr const char* PRETTY_NAME = "pname";
    static constexpr const char* IMPL_CLASS = "implcls";
    static constexpr const char* METHOD_IS_VALID = "IsValid";
};

}
