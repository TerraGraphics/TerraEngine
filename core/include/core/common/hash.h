#pragma once

#include <functional>


template <typename T, typename Tag> struct NamedType;
namespace std {
    template <typename T, typename Tag> struct hash<NamedType<T, Tag>> {
        size_t operator()(const NamedType<T, Tag>& x) const {
            return std::hash<T>()(x.GetRealValue());
        }
    };
}

template <typename T> inline void HashCombine(std::size_t& seed, const T& v) {
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

template <typename T> struct ContainerHasher {
    std::size_t operator()(const T& container) const {
        std::size_t hash = 0;
        for (const auto& v : container) {
            HashCombine(hash, v);
        }

        return hash;
    }
};
