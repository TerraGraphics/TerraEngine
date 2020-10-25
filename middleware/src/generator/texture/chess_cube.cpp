#include "middleware/generator/texture/chess_cube.h"

#include "core/math/numbers.h"


math::Generator3d ChessCube::Result() const {
    return math::Generator3d([](double x, double y, double z) -> double {
        int32_t ix = math::ToInt32Continuous(x);
        int32_t iy = math::ToInt32Continuous(y);
        int32_t iz = math::ToInt32Continuous(z);
        return ((ix & 1) ^ (iy & 1) ^ (iz & 1)) ? -1.: 1.;
    });
}
