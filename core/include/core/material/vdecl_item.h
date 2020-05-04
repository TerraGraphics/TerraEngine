#pragma once

#include <string>
#include <cstdint>
#include <cstddef>


enum class VDeclType : uint8_t {
    Float = 0,
    Float2 = 1,
    Float3 = 2,
    Float4 = 3,
    Color3 = 4,
    Color4 = 5,
};

struct VDeclItem {
    VDeclItem() = delete;
    VDeclItem(const std::string& varName, VDeclType varType, uint32_t bufferSlot = 0, bool perVertex = true);
    ~VDeclItem() = default;

    bool operator==(const VDeclItem& other) const;

    size_t Hash() const;

public:
    std::string varName;
    uint32_t bufferSlot = 0;
    VDeclType varType = VDeclType::Float;
    bool perVertex = true;
};
