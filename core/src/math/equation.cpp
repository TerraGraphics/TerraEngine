#include "core/math/equation.h"

#include <cmath>
#include <algorithm>

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

uint8_t SolveCubic(double a, double b, double c, double* result) {
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
        double alpha = -std::cbrt(std::fabs(r) + std::sqrt(r2 - q3));
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

uint8_t SolveCubic(float a, float b, float c, float* result) {
    double res[3];
    auto rootsCnt = SolveCubic(static_cast<double>(a), static_cast<double>(b), static_cast<double>(c), res);
    for (uint8_t i=0; i!=rootsCnt; ++i) {
        result[i] = static_cast<float>(res[i]);
    }

    return rootsCnt;
}

uint8_t SolveCubic(double a, double b, double c, double d, double* result) {
    if (std::fpclassify(a) != FP_ZERO) {
        return SolveCubic(b / a, c / a, d / a, result);
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

/*
    x^4 + a*x^3 + b*x^2 + c*x + d = (x^2 + p1*x + q1)*(x^2 + p2*x + q2)
    =>
    a = p2 + p1
    b = q2 + q1 + p2*p1
    c = p1*q2 + p2*q1
    d = q1*q2
    => (see https://en.wikipedia.org/wiki/Vieta%27s_formulas)
    p*p - a*p + b - w = 0
    q*q - w*q + d = 0
    where: w = q1 + q2
    =>
    dp = sqrt(a*a - 4*b + 4*w))
    p1,2 = (a +/- dp)/2
    dq = sqrt(w*w - 4*d)
    q1,2 = (w -/+ dq)/2
    =>
    c = p1*q2 + p2*q1 = ((a + dp) * (w + dq) + (a - dp) * (w - dq)) / 4
    =>
    cube resolvent: w^3 - b*w^2 + (ca - 4*d)*w - d*a*a + 4*d*b - c*c = 0
    w = max(|w1|, |w2|, |w3|)
*/
uint8_t SolveQuartic(double a, double b, double c, double d, double* result) {
    double w;
    {
        double a3 = -b;
        double b3 =  a*c -4.*d;
        double c3 = -a*a*d + 4.*b*d - c*c;

        double w3[3];
        auto rootsCnt = SolveCubic(a3, b3, c3, w3);
        if (rootsCnt == 0) {
            return 0;
        }

        w = w3[0];
        double maxAbs = std::fabs(w);
        for (uint8_t i=1; i!=rootsCnt; ++i) {
            if (maxAbs < std::fabs(w3[i])) {
                w = w3[i];
                maxAbs = std::fabs(w);
            }
        }
    }

    double q[2];
    double p[2];

    // solving quad: q*q - w*q + d = 0
    if (SolveQuad(-w, d, q) != 2) {
        return 0;
    }

    if (std::fpclassify(q[0] - q[1]) != FP_ZERO) {
        /*
            https://en.wikipedia.org/wiki/Cramer%27s_rule
               p1 +    p2 = a
            q2*p1 + q1*p2 = c
            =>
            p1 = (q1*a - c) / (q1 - q2)
            p2 = (c - q2*a) / (q1 - q2) or p2 = a - p1
        */
        p[0] = (q[0] * a - c) / (q[0] - q[1]);
        p[1] = a - p[0];
    } else {
        q[0] = q[1] = w * 0.5;
        // solving quad: p*p - a*p + b - w = 0
        if (SolveQuad(-a, b - w, p) != 2) {
            return 0;
        }
    }

    // solving quad: x*x + p1*x + q1 = 0
    uint8_t rootsCnt = SolveQuad(p[0], q[0], result);

    // solving quad: x*x + p2*x + q2 = 0
    rootsCnt += SolveQuad(p[1], q[1], &result[rootsCnt]);

    return rootsCnt;
}

uint8_t SolveQuartic(float a, float b, float c, float d, float* result) {
    double res[4];
    auto rootsCnt = SolveQuartic(
        static_cast<float>(a), static_cast<double>(b), static_cast<double>(c), static_cast<double>(d), res);
    for (uint8_t i=0; i!=rootsCnt; ++i) {
        result[i] = static_cast<float>(res[i]);
    }

    return rootsCnt;
}

uint8_t SolveQuartic(double a, double b, double c, double d, double e, double* result) {
    if (std::fpclassify(a) != FP_ZERO) {
        return SolveQuartic(b / a, c / a, d / a, e / a, result);
    }

    return SolveCubic(b, c, d, e, result);
}

uint8_t SolveQuartic(float a, float b, float c, float d, float e, float* result) {
    double res[4];
    auto rootsCnt = SolveQuartic(
        static_cast<float>(a), static_cast<double>(b), static_cast<double>(c), static_cast<double>(d), static_cast<double>(e), res);
    for (uint8_t i=0; i!=rootsCnt; ++i) {
        result[i] = static_cast<float>(res[i]);
    }

    return rootsCnt;
}

}
