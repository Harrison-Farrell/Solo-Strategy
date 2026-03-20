// -----------------------------------------------------------------------------
// Author:      Harrison Farrell
// Project:     Solo-Strategy Trading System
// Copyright:   (c) 2026 Harrison Farrell. All Rights Reserved.
//
// Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
// This program is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See <https://www.gnu.org/licenses/agpl-3.0.html> for full details.
// -----------------------------------------------------------------------------

/// @file endian_utils.h
/// @brief Utilities for cross-platform endian conversion.
///
/// Provides template-based functions to safely Swap bytes and convert
/// big/little-endian data to host-native format.

#ifndef SOLO_STRATEGY_SRC_ITCH_PARSER_ENDIAN_UTILS_H_
#define SOLO_STRATEGY_SRC_ITCH_PARSER_ENDIAN_UTILS_H_

#include <bit>
#include <concepts>
#include <cstdint>

/// @namespace endian
/// @brief Tools for handling different binary data orderings.
namespace endian {

/// @brief Swaps the byte order of the given value.
/// @tparam T An integral type (uint16_t, uint32_t, etc.).
/// @param value The value to Swap.
/// @return The value with reversed byte order.
template <std::integral T>
constexpr T Swap(T value) noexcept {
    return std::byteswap(value);
}

/// @brief Converts a Big-Endian value to the host's native endianness.
///
/// @tparam T An integral type.
/// @param value Big-endian input value.
/// @return Host-native representation.
template <std::integral T>
constexpr T FromBigEndian(T value) noexcept {
    if constexpr (std::endian::native == std::endian::little) {
        return Swap(value);
    } else {
        return value;
    }
}

/// @brief Converts a Little-Endian value to the host's native endianness.
///
/// @tparam T An integral type.
/// @param value Little-endian input value.
/// @return Host-native representation.
template <std::integral T>
constexpr T FromLittleEndian(T value) noexcept {
    if constexpr (std::endian::native == std::endian::big) {
        return Swap(value);
    } else {
        return value;
    }
}

}  // namespace endian

#endif  // SOLO_STRATEGY_SRC_ITCH_PARSER_ENDIAN_UTILS_H_
