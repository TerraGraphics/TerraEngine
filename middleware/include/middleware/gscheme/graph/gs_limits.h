#pragma once

#include <limits>
#include <cstdint>


namespace gs {
    constexpr const uint8_t MAX_PINS_COUNT = std::numeric_limits<uint8_t>::max() - 1;
    constexpr const uint16_t MAX_NODES_COUNT = std::numeric_limits<uint16_t>::max() - 1;
    constexpr const uint16_t INVALID_NODE_INDEX = std::numeric_limits<uint16_t>::max();
    constexpr const uint16_t INVALID_ORDER_VALUE = std::numeric_limits<uint16_t>::max();
    constexpr const uint16_t INVALID_TYPE_CLASS_INDEX = std::numeric_limits<uint16_t>::max();
}
