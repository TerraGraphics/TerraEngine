#pragma once

#include <new>
#include <utility>


template<class T, std::size_t Size, std::size_t Alignment> class Pimpl {
public:
    template<class...Args> explicit Pimpl(Args&&... args) {
        new (RawPtr()) T(std::forward<Args>(args)...);
    }

    Pimpl(const Pimpl& other) {
        new (RawPtr()) T(*other);
    }

    Pimpl(Pimpl&& other) noexcept = default;

    ~Pimpl() noexcept {
        Validate<sizeof(T), alignof(T)>();
        RawPtr()->~T();
    }

    Pimpl& operator=(Pimpl&& rhs) noexcept {
        *RawPtr() = std::move(*rhs);
        return *this;
    }

    T* RawPtr() noexcept {return reinterpret_cast<T*>(&m_data);}
    const T* RawPtr() const noexcept {return reinterpret_cast<const T*>(&m_data);}

    T* operator->() noexcept { return RawPtr(); }
    const T* operator->() const noexcept { return RawPtr(); }

    T& operator*() noexcept { return *RawPtr(); }
    const T& operator*() const noexcept { return *RawPtr(); }

private:
    template<std::size_t ActualSize, std::size_t ActualAlignment>
        static void Validate() noexcept {
            static_assert(Size == ActualSize, "Size and sizeof(T) mismatch");
            static_assert(Alignment == ActualAlignment,"Alignment and alignof(T) mismatch");
        }

private:
    std::aligned_storage_t<Size, Alignment> m_data;
};
