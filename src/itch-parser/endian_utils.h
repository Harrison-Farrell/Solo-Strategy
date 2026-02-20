#ifndef SOLO_STRATEGY_SRC_ITCH_PARSER_ENDIAN_UTILS_H_
#define SOLO_STRATEGY_SRC_ITCH_PARSER_ENDIAN_UTILS_H_

#include <bit>
#include <concepts>

namespace endian {

/**
 * @brief Swaps the byte order of the given value.
 * Uses std::byteswap if C++23 is available, otherwise provides a fallback.
 */
template <std::integral T>
constexpr T swap(T value) noexcept {
#if __cpp_lib_byteswap
    return std::byteswap(value);
#else
    // Fallback for older compilers if C++23 is not fully supported
    if constexpr (sizeof(T) == 1) {
        return value;
    } else if constexpr (sizeof(T) == 2) {
        return static_cast<T>((value << 8) | (value >> 8));
    } else if constexpr (sizeof(T) == 4) {
        uint32_t v = static_cast<uint32_t>(value);
        v = ((v << 8) & 0xFF00FF00) | ((v >> 8) & 0x00FF00FF);
        return static_cast<T>((v << 16) | (v >> 16));
    } else if constexpr (sizeof(T) == 8) {
        uint64_t v = static_cast<uint64_t>(value);
        v = ((v << 8) & 0xFF00FF00FF00FF00ULL) | ((v >> 8) & 0x00FF00FF00FF00FFULL);
        v = ((v << 16) & 0xFFFF0000FFFF0000ULL) | ((v >> 16) & 0x0000FFFF0000FFFFULL);
        return static_cast<T>((v << 32) | (v >> 32));
    } else {
        return value;
    }
#endif
}

/**
 * @brief Converts a Big-Endian value to host endianness.
 */
template <std::integral T>
constexpr T from_big_endian(T value) noexcept {
    if constexpr (std::endian::native == std::endian::little) {
        return swap(value);
    } else {
        return value;
    }
}

/**
 * @brief Converts a Little-Endian value to host endianness.
 */
template <std::integral T>
constexpr T from_little_endian(T value) noexcept {
    if constexpr (std::endian::native == std::endian::big) {
        return swap(value);
    } else {
        return value;
    }
}

}  // namespace endian

#endif  // SOLO_STRATEGY_SRC_ITCH_PARSER_ENDIAN_UTILS_H_
