#include "middleware/gscheme/meta/gs_format.h"

#include <cmath>

#include "fmt/fmt.h"


namespace gs {

std::string ToString(double value, int maxExp, uint8_t maxPrecision) {
    uint8_t precision = 0;
    auto absValue = std::abs(value);
    for (int i=maxExp; i>0; i--) {
        if (absValue >= std::pow(10., static_cast<double>(i))) {
            break;
        }
        ++precision;
    }

    precision = std::min(precision, maxPrecision);
    return fmt::format("{:." + std::to_string(precision) + "f}", value);
}

std::string ToString(int64_t value) {
    return std::to_string(value);
}

std::string ToString(uint64_t value) {
    return std::to_string(value);
}

}
