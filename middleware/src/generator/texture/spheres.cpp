#include "middleware/generator/texture/spheres.h"

#include <cmath>
#include <algorithm>

#include "core/math/generator_type.h"


math::Generator3D Spheres::Result() const {
    return math::Generator3D([frequency = m_frequency](double x, double y, double z) -> double {
        double distToCenter = std::hypot(x, y, z) * frequency;
        double distToInnerSphere = distToCenter - std::floor(distToCenter);  // [0, 1)
        double distToOuterSphere = 1. - distToInnerSphere;                   // (0, 1]
        double nearestDist = std::min(distToInnerSphere, distToOuterSphere); // [0, 0.5]
        return 1. - 4. * nearestDist;                                        // [-1, 1]
    });
}
