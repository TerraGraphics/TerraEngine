#pragma once

#include <cstdint>

namespace math {

    /*
        Solve linear equation: a*x + b = 0
        Param: result - array of size 1
        Return count count of root [0; 1]
    */
    uint8_t SolveLinear(double a, double b, double* result);
    uint8_t SolveLinear(float a, float b, float* result);

    /*
        Solve quad equation: x*x + a*x + b = 0
        Param: result - array of size 2
        Return count count of root [0; 2]
    */
    uint8_t SolveQuad(double a, double b, double* result);
    uint8_t SolveQuad(float a, float b, float* result);

    /*
        Solve quad equation: a*x*x + b*x + c = 0
        Param: result - array of size 2
        Return count count of root [0; 2]
    */
    uint8_t SolveQuad(double a, double b, double c, double* result);
    uint8_t SolveQuad(float a, float b, float c, float* result);

    /*
        Solve cubic equation: a*x*x*x + b*x*x + c*x + d = 0 (see http://algolist.ru/maths/findroot/cubic.php)
        Param: result - array of size 3
        Return count count of root [0; 3]
    */
    uint8_t SolveCubic(double a, double b, double c, double d, double* result);
    uint8_t SolveCubic(float a, float b, float c, float d, float* result);
}
