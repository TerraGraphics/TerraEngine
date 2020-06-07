#pragma once

#ifndef _MSC_VER
#   include <cxxabi.h>
#endif
#include <memory>
#include <string>
#include <cstdlib>
#include <typeinfo>
#include <type_traits>


namespace meta {

// example: TypeName<double>()
template <typename T> std::string TypeName() {
    typedef typename std::remove_reference<T>::type TR;
    std::unique_ptr<char, void(*)(void*)> own
           (
#ifndef _MSC_VER
                abi::__cxa_demangle(typeid(TR).name(), nullptr,
                                           nullptr, nullptr),
#else
                nullptr,
#endif
                std::free
           );
    std::string r = own != nullptr ? own.get() : typeid(TR).name();
    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}

template <typename...>
    using void_t = void;

template <typename, template <typename> class, typename = void_t<>>
    struct Detect : std::false_type {};

template <typename T, template <typename> class Op>
    struct Detect<T, Op, void_t<Op<T>>> : std::true_type {};

template <typename T>
    using ArrayLikeT = decltype(std::declval<T>()[0]);

template <typename T>
    using IsArrayLike = Detect<T, ArrayLikeT>;

template <typename T>
  inline constexpr bool IsArrayLikeV = IsArrayLike<T>::value;

}
