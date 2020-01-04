#pragma once

#include <chrono>

class Timer {
public:
    void Start();
    // In seconds
    double TimePoint();

private:
    std::chrono::high_resolution_clock::time_point m_start;
};
