#pragma once

#include <cmath>


namespace math {

inline int32_t ToInt32Continuous(double v) {
    using inT = double;
    using outT = int32_t;
    constexpr const inT maxf = static_cast<inT>(std::numeric_limits<outT>::max());
    constexpr const inT minf = static_cast<inT>(std::numeric_limits<outT>::min());
    constexpr const inT countf = maxf - minf + static_cast<inT>(1);

    v = floor(v);
    if (v > maxf) {
        return static_cast<outT>(fmod(v - minf, countf) + minf);
    } else if (v < minf) {
        return static_cast<outT>(countf + fmod(v - minf, countf) + minf);
    } else {
        return static_cast<outT>(v);
    }
}

}
