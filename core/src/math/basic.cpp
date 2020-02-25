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

}