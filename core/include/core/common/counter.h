#pragma once

#include <cstdint>
#include <cstddef>


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

template<typename T, size_t SIZE> class CyclicalCounter {
public:
    CyclicalCounter() = default;
    CyclicalCounter(T initialValue, size_t recalcIndex = SIZE * 100)
        : m_total(initialValue * static_cast<T>(SIZE))
        , m_recalcIndex(recalcIndex) {
        for (size_t i=0; i!=SIZE; ++i) {
            m_deltas[i] = initialValue;
        }
    }

    const T Sum() const noexcept { return m_total; }
    const T Avg() const noexcept { return m_total / static_cast<T>(SIZE); }
    size_t Size() const noexcept { return SIZE; }
    size_t Index() const noexcept { return m_index; }

    void Add(T value) noexcept {
        const size_t ind = m_index++ % SIZE;
        m_total += (value - m_deltas[ind]);
        m_deltas[ind] = value;
        if (m_index % m_recalcIndex == 0) {
            m_total = T(0);
            for (size_t i=0; i!=SIZE; ++i) {
                m_total += m_deltas[i];
            }
        }
    }

private:
    T m_deltas[SIZE] = { T(0) };
    T m_total = T(0);
    size_t m_index = 0;
    size_t m_recalcIndex = SIZE * 100;
};
