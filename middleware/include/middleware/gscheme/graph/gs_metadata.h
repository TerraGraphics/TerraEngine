#pragma once

#include <cstdint>


namespace gs {

enum class MetaTypes : uint8_t {
    CLASS = 0,
    INPUT_PIN = 1,
    OUTPUT_PIN = 2,
    EMBEDDED_PROPERTY = 3,
};

enum class PinTypes : uint8_t {
    INPUT = 0,
    OUTPUT = 1,
    EMBEDDED = 2,
};

struct MetaNames {
    static constexpr const char* CLASS = "cls";
    static constexpr const char* PIN = "pin";
    static constexpr const char* TYPE = "type";
    static constexpr const char* PRETTY_NAME = "pname";
};

}
