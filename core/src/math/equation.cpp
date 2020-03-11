#include "core/math/equation.h"

#include <cmath>


namespace math {

uint8_t SolveLinear(double a, double b, double* result) {
    if (std::fpclassify(a) != FP_ZERO) {
        result[0] = - b / a;
        return 1;
    }

    return 0;
}

uint8_t SolveLinear(float a, float b, float* result) {
    double res[1];
    auto rootsCnt = SolveLinear(static_cast<float>(a), static_cast<float>(b), res);
    for (uint8_t i=0; i!=rootsCnt; ++i) {
        result[i] = static_cast<float>(res[i]);
    }

    return rootsCnt;
}

uint8_t SolveQuad(double a, double b, double c, double* result) {
    if (std::fpclassify(a) != FP_ZERO) {
        auto d = b * b - 4. * a * c;
        if (d < 0) {
            return 0;
        }
        d = std::sqrt(d);
        result[0] = (-b + d) * 0.5 / a;
        result[1] = (-b - d) * 0.5 / a;

        return 2;
    }

    return SolveLinear(b, c, result);
}

uint8_t SolveQuad(float a, float b, float c, float* result) {
    double res[2];
    auto rootsCnt = SolveQuad(static_cast<float>(a), static_cast<float>(b), static_cast<float>(c), res);
    for (uint8_t i=0; i!=rootsCnt; ++i) {
        result[i] = static_cast<float>(res[i]);
    }

    return rootsCnt;
}

}
