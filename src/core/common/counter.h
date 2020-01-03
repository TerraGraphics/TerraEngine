#pragma once

#include <cstdint>


template<typename T> class Counter {
public:
    Counter() : m_id(++Counter::m_lastId) {}

    uint32_t GetId() const noexcept { return m_id; }

private:
    uint32_t m_id = 0;

private:
    static uint32_t m_lastId;
};

template <class T> uint32_t Counter<T>::m_lastId = 0;
