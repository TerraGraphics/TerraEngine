#pragma once

#include <string>


namespace gs {

std::string ToString(double value, int maxExp, uint8_t maxPrecision);
std::string ToString(int64_t value);
std::string ToString(uint64_t value);

}
