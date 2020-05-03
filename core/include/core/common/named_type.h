#pragma once

#include <utility>


template <typename T, typename Tag> struct NamedType {
    template <typename Enable = typename std::is_default_constructible<T>::type>
        NamedType() : m_value() {}

    template <typename Enable = typename std::is_copy_constructible<T>::type>
        explicit NamedType(const T& other) : m_value(other) {}
    NamedType(const NamedType&) = default;

    template <typename Enable = typename std::is_move_constructible<T>::type>
        explicit NamedType(T&& other) : m_value(std::move(other)) {}
    NamedType(NamedType&&) = default;

    template <typename Enable = typename std::is_copy_assignable<T>::type>
        NamedType& operator=(const T& other) { m_value = other; return *this; }
    NamedType& operator=(const NamedType&) = default;

    template <typename Enable = typename std::is_move_assignable<T>::type>
        NamedType& operator=(T&& other) { m_value = std::move(other); return *this;}
    NamedType& operator=(NamedType&&) = default;

    operator const T& () const { return m_value; }
    operator T& () { return m_value; }

    bool operator==(const NamedType& other) const { return (m_value == other.m_value); }
    bool operator<(const NamedType& other) const { return (m_value < other.m_value); }

    T& GetRealValue() { return m_value; }
    const T& GetRealValue() const { return m_value; }

private:
    T m_value;
};
