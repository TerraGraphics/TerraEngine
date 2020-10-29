#pragma once

#include <string>

#include "fmt/fmt.h"
#include "middleware/gscheme/graph/gs_types_decl.h"


template <> struct fmt::formatter<gs::TypeId> : formatter<std::string> {

template <typename FormatContext>
    auto format(gs::TypeId v, FormatContext& ctx) {
        if (v == gs::TypeId::UniversalType) {
            return formatter<std::string>::format("universal", ctx);
        }

        std::string name;
        switch (gs::ToBaseTypeId(v)) {
            case gs::TypeId::Int8: name = "int8"; break;
            case gs::TypeId::Uint8: name = "uint8"; break;
            case gs::TypeId::Int16: name = "int16"; break;
            case gs::TypeId::Uint16: name = "uint16"; break;
            case gs::TypeId::Int32: name = "int32"; break;
            case gs::TypeId::Uint32: name = "uint32"; break;
            case gs::TypeId::Int64: name = "int64"; break;
            case gs::TypeId::Uint64: name = "uint64"; break;
            case gs::TypeId::Float: name = "float"; break;
            case gs::TypeId::Double: name = "double"; break;
            case gs::TypeId::Vector2f: name = "vector2f"; break;
            case gs::TypeId::Vector3f: name = "vector3f"; break;
            case gs::TypeId::Vector4f: name = "vector4f"; break;
            case gs::TypeId::Generator2d: name = "generator2d"; break;
            case gs::TypeId::Generator3d: name = "generator3d"; break;
            case gs::TypeId::String: name = "string"; break;
            default: name = "unknown"; break;
        }

        if (gs::HasUniversalBit(v)) {
            return formatter<std::string>::format("universal-" + name, ctx);
        }

        return formatter<std::string>::format(name, ctx);
    }

};
