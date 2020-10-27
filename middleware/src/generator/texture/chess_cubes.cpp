#include "middleware/generator/texture/chess_cubes.h"

#include "core/math/numbers.h"
#include "core/common/exception.h"
#include "core/math/generator_type.h"


math::Generator3D ChessCubes::Result() const {
    return math::Generator3D([size = m_size](double x, double y, double z) -> double {
        int32_t ix = math::ToInt32Continuous(x / size);
        int32_t iy = math::ToInt32Continuous(y / size);
        int32_t iz = math::ToInt32Continuous(z / size);
        return ((ix & 1) ^ (iy & 1) ^ (iz & 1)) ? -1.: 1.;
    });
}

void ChessCubes::SetSize(const float v) {
    // TODO: add conditional to UI
    if (v <= 0) {
        throw EngineError("ChessCubes: side size must be greater than zero");
    }
    m_size = v;
}
