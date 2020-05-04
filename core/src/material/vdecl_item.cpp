#include "core/material/vdecl_item.h"

#include <cstdint>

#include "core/common/hash.h"
#include "core/common/exception.h"


VDeclItem::VDeclItem(const std::string& varName, VDeclType varType, uint32_t bufferSlot, bool perVertex)
    : varName(varName)
    , bufferSlot(bufferSlot)
    , varType(varType)
    , perVertex(perVertex) {
    if ((varType < VDeclType::Float) || (varType > VDeclType::Color4)) {
        throw EngineError("VDeclItem: wrong varType value = {}", varType);
    }
}

bool VDeclItem::operator==(const VDeclItem& other) const {
    return (
        (bufferSlot == other.bufferSlot) &&
        (varType == other.varType) &&
        (perVertex == other.perVertex) &&
        (varName == other.varName));
}

size_t VDeclItem::Hash() const {
    auto hash = std::hash<std::string>()(varName);

    auto val = (
        (static_cast<uint64_t>(bufferSlot) << uint64_t(32)) |
        (static_cast<uint64_t>(varType) << uint64_t(16)) |
        static_cast<uint64_t>(perVertex));
    HashCombine(hash, val);

    return hash;
}
