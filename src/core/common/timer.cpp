#include "core/common/timer.h"


void Timer::Start() {
    m_start = std::chrono::high_resolution_clock().now();
}

double Timer::TimePoint() {
    auto now = std::chrono::high_resolution_clock().now();
    auto dt = std::chrono::duration_cast<std::chrono::duration<double>>(now - m_start).count();
    m_start = now;

    return dt;
}
