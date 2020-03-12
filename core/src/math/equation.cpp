#include "core/math/equation.h"

#include <cmath>

#include "core/math/constants.h"


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
    auto rootsCnt = SolveLinear(static_cast<double>(a), static_cast<double>(b), res);
    for (uint8_t i=0; i!=rootsCnt; ++i) {
        result[i] = static_cast<float>(res[i]);
    }

    return rootsCnt;
}

uint8_t SolveQuad(double a, double b, double* result) {
    a *= 0.5;
    auto d = a * a - b;
    if (d < 0) {
        return 0;
    }
    d = std::sqrt(d);
    result[0] = -a + d;
    result[1] = -a - d;

    return 2;
}

uint8_t SolveQuad(float a, float b, float* result) {
    double res[2];
    auto rootsCnt = SolveQuad(static_cast<double>(a), static_cast<double>(b), res);
    for (uint8_t i=0; i!=rootsCnt; ++i) {
        result[i] = static_cast<float>(res[i]);
    }

    return rootsCnt;
}

uint8_t SolveQuad(double a, double b, double c, double* result) {
    if (std::fpclassify(a) != FP_ZERO) {
        b *= 0.5;
        auto d = b * b - a * c;
        if (d < 0) {
            return 0;
        }
        d = std::sqrt(d);
        result[0] = (-b + d) / a;
        result[1] = (-b - d) / a;

        return 2;
    }

    return SolveLinear(b, c, result);
}

uint8_t SolveQuad(float a, float b, float c, float* result) {
    double res[2];
    auto rootsCnt = SolveQuad(static_cast<double>(a), static_cast<double>(b), static_cast<double>(c), res);
    for (uint8_t i=0; i!=rootsCnt; ++i) {
        result[i] = static_cast<float>(res[i]);
    }

    return rootsCnt;
}

uint8_t SolveCubic(double a, double b, double c, double d, double* result) {
    if (std::fpclassify(a) != FP_ZERO) {
        // x*x*x + a*x*x + b*x + c = 0
        double invA = 1. / a;
        a = b * invA;
        b = c * invA;
        c = d * invA;

        double a2 = a*a;
        double q  = (a2 - 3. * b) / 9.;
        double r  = (a * (2. * a2 - 9. * b) + 27. * c) / 54.;
        double r2 = r*r;
        double q3 = q*q*q;

        if (q3 > r2) {
            double t = std::acos(std::min(std::max(r / std::sqrt(q3), -1.), 1.)) / 3.;
            a /= 3.;
            q = -2.*std::sqrt(q);

            result[0] = q * std::cos(t) - a;
            result[1] = q * std::cos(t + OneThirdOfTwoPI<double>()) - a;
            result[2] = q * std::cos(t - OneThirdOfTwoPI<double>()) - a;

            return 3;

        } else {
            double alpha = -std::pow(std::fabs(r) + std::sqrt(r2 - q3), 1. / 3.);
            if (r < 0) {
                alpha = -alpha;
            }

            if (std::fpclassify(alpha) != FP_ZERO) {
                result[0] = alpha + q / alpha - a / 3.;
            } else {
                result[0] = - a / 3.;
            }

            return 1;
        }
    }

    return SolveQuad(b, c, d, result);
}

uint8_t SolveCubic(float a, float b, float c, float d, float* result) {
    double res[3];
    auto rootsCnt = SolveCubic(static_cast<double>(a), static_cast<double>(b), static_cast<double>(c), static_cast<double>(d), res);
    for (uint8_t i=0; i!=rootsCnt; ++i) {
        result[i] = static_cast<float>(res[i]);
    }

    return rootsCnt;
}
    for (uint8_t i=0; i!=rootsCnt; ++i) {
        result[i] = static_cast<float>(res[i]);
    }

    return rootsCnt;
}

}
