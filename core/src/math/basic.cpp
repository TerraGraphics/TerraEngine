#include "core/math/basic.h"


namespace math {

Axis Next(Axis value) {
    switch (value) {
    case Axis::X:
        return Axis::Y;
    case Axis::Y:
        return Axis::Z;
    default:
        return Axis::X;
    }
}

Axis Prev(Axis value) {
    switch (value) {
    case Axis::X:
        return Axis::Z;
    case Axis::Y:
        return Axis::X;
    default:
        return Axis::Y;
    }
}

Axis GetThirdAxis(Axis first, Axis second) {
    const constexpr uint8_t sum = static_cast<uint8_t>(Axis::X) + static_cast<uint8_t>(Axis::Y) + static_cast<uint8_t>(Axis::Z);
    return static_cast<Axis>(sum - static_cast<uint8_t>(first) - static_cast<uint8_t>(second));
}

}
