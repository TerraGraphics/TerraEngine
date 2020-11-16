#pragma once

#include <array>
#include <cstddef>


namespace gui {
namespace detail {

bool Combo(const char* label, size_t& currentIndex, const char* const* itemNames, const size_t numberItems);

} // end namespace gui::detail

template<typename T, size_t N>
    bool Combo(const char* label, T& value, const std::array<const char*, N>& itemNames) {
        size_t currentIndex = static_cast<size_t>(value);
        if (detail::Combo(label, currentIndex, itemNames.begin(), itemNames.size())) {
            value = static_cast<T>(currentIndex);
            return true;
        }

        return false;
    }

} // end namespace gui
