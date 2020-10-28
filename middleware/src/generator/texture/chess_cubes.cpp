#include "middleware/generator/texture/chess_cubes.h"

#include "core/math/numbers.h"
#include "core/common/exception.h"
#include "core/math/generator_type.h"


math::Generator3D ChessCubes::Result() const {
    return math::Generator3D([frequency = m_frequency](double x, double y, double z) -> double {
        int32_t ix = math::ToInt32Continuous(x * frequency);
        int32_t iy = math::ToInt32Continuous(y * frequency);
        int32_t iz = math::ToInt32Continuous(z * frequency);
        return ((ix & 1) ^ (iy & 1) ^ (iz & 1)) ? -1.: 1.;
    });
}
