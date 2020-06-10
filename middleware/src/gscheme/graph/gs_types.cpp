#include "middleware/gscheme/graph/gs_types.h"


namespace gs {

TypeId MaxTypeId(TypeId a, TypeId b) {
    constexpr const uint8_t mask = (static_cast<uint8_t>(TypeId::UniversalType) - uint8_t(1));
    uint8_t aVal = static_cast<uint8_t>(a) & mask;
    uint8_t bVal = static_cast<uint8_t>(b) & mask;

    return static_cast<TypeId>((aVal > bVal) ? aVal : bVal);
}

}
