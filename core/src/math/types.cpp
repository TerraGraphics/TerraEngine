#include "core/math/types.h"


namespace math {

Axis Next(Axis value) noexcept {
    switch (value) {
    case Axis::X:
        return Axis::Y;
    case Axis::Y:
        return Axis::Z;
    default:
        return Axis::X;
    }
}

Axis Prev(Axis value) noexcept {
    switch (value) {
    case Axis::X:
        return Axis::Z;
    case Axis::Y:
        return Axis::X;
    default:
        return Axis::Y;
    }
}

Axis GetThirdAxis(Axis first, Axis second) noexcept {
    const constexpr uint8_t sum = static_cast<uint8_t>(Axis::X) + static_cast<uint8_t>(Axis::Y) + static_cast<uint8_t>(Axis::Z);
    return static_cast<Axis>(sum - static_cast<uint8_t>(first) - static_cast<uint8_t>(second));
}

Axis GetThirdAxis(Axis2 values) noexcept {
    const constexpr uint8_t sum = static_cast<uint8_t>(Axis::X) + static_cast<uint8_t>(Axis::Y) + static_cast<uint8_t>(Axis::Z);
    return static_cast<Axis>(sum - static_cast<uint8_t>(values[0]) - static_cast<uint8_t>(values[1]));
}

Axis GetAxis(Direction value) noexcept {
    return static_cast<Axis>(static_cast<uint8_t>(value) % uint8_t(3));
}

Direction GetDirection(Axis value) noexcept {
    return static_cast<Direction>(value);
}

int8_t GetSign(Direction value) noexcept {
    return (value <= Direction::POS_Z) ? 1 : -1;
}

}
