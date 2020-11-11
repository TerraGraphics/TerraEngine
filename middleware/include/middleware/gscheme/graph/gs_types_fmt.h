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
            case gs::TypeId::Float: name = "float"; break;
            case gs::TypeId::Vector2f: name = "vector2f"; break;
            case gs::TypeId::Vector3f: name = "vector3f"; break;
            case gs::TypeId::Vector4f: name = "vector4f"; break;
            case gs::TypeId::Generator2d: name = "generator2d"; break;
            case gs::TypeId::Generator3d: name = "generator3d"; break;
            default: name = "unknown"; break;
        }

        if (gs::HasUniversalBit(v)) {
            return formatter<std::string>::format("universal-" + name, ctx);
        }

        return formatter<std::string>::format(name, ctx);
    }

};
