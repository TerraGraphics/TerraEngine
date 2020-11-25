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
    static constexpr const char* PIN = "pin";
    static constexpr const char* PIN_TYPE = "ptype";
    static constexpr const char* TYPE_INSTANCE = "itype";
    static constexpr const char* DISPLAY_NAME = "dname";
};

}
