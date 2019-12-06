#pragma once

class Noncopyable {
protected:
    Noncopyable() = default;
    ~Noncopyable() = default;

    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
};

class Nonmoveable {
protected:
    Nonmoveable() = default;
    ~Nonmoveable() = default;

    Nonmoveable(Nonmoveable&&) noexcept = delete;
    Nonmoveable& operator=(Nonmoveable&&) noexcept = delete;
};

class Fixed {
protected:
    Fixed() = default;
    ~Fixed() = default;

    Fixed(const Fixed&) = delete;
    Fixed(Fixed&&) noexcept = delete;
    Fixed& operator=(const Fixed&) = delete;
    Fixed& operator=(Fixed&&) noexcept = delete;
};
