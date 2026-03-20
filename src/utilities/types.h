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

/// @file types.h
/// @brief Global type definitions and constants for the Solo-Strategy project.

#ifndef SOLO_STRATEGY_SRC_UTILITIES_TYPES_H_
#define SOLO_STRATEGY_SRC_UTILITIES_TYPES_H_

#include <cstdint>
#include <limits>
#include <string>

/// @brief Maximum number of trading instruments/tickers.
constexpr size_t ME_MAX_TICKERS = 8;

/// @brief Maximum price level depth in the order books.
constexpr size_t ME_MAX_PRICE_LEVELS = 256;

/// @brief Maximum number of trading clients.
constexpr size_t ME_MAX_NUM_CLIENTS = 256;

/// @brief Maximum number of orders per trading client.
constexpr size_t ME_MAX_ORDER_IDS = static_cast<size_t>(1024) * 1024;

/// @brief Maximum size of lock-free queues for client updates.
constexpr size_t ME_MAX_CLIENT_UPDATES = static_cast<size_t>(256) * 1024;

/// @brief Maximum size of lock-free queues for market updates.
constexpr size_t ME_MAX_MARKET_UPDATES = static_cast<size_t>(256) * 1024;

/// @brief Type alias for OrderId.
using OrderId = uint64_t;

/// @brief Invalid value marker for OrderId.
constexpr auto OrderId_INVALID = std::numeric_limits<OrderId>::max();

/// @brief Converts an OrderId to its string representation.
/// @param order_id The OrderId to convert.
/// @return String representation, or "INVALID".
inline auto OrderIdToString(OrderId order_id) -> std::string {
    if (order_id == OrderId_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(order_id);
}

/// @brief Type alias for TickerId.
using TickerId = uint32_t;

/// @brief Invalid value marker for TickerId.
constexpr auto TickerId_INVALID = std::numeric_limits<TickerId>::max();

/// @brief Converts a TickerId to its string representation.
/// @param ticker_id The TickerId to convert.
/// @return String representation, or "INVALID".
inline auto TickerIdToString(TickerId ticker_id) -> std::string {
    if (ticker_id == TickerId_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(ticker_id);
}

/// @brief Type alias for ClientId.
using ClientId = uint32_t;

/// @brief Invalid value marker for ClientId.
constexpr auto ClientId_INVALID = std::numeric_limits<ClientId>::max();

/// @brief Converts a ClientId to its string representation.
/// @param client_id The ClientId to convert.
/// @return String representation, or "INVALID".
inline auto ClientIdToString(ClientId client_id) -> std::string {
    if (client_id == ClientId_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(client_id);
}

/// @brief Type alias for Price. Signed as negative prices are possible in some contexts.
using Price = int64_t;

/// @brief Invalid value marker for Price.
constexpr auto Price_INVALID = std::numeric_limits<Price>::max();

/// @brief Converts a Price to its string representation.
/// @param price The Price to convert.
/// @return String representation, or "INVALID".
inline auto PriceToString(Price price) -> std::string {
    if (price == Price_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(price);
}

/// @brief Type alias for Quantity.
using Qty = uint32_t;

/// @brief Invalid value marker for Qty.
constexpr auto Qty_INVALID = std::numeric_limits<Qty>::max();

/// @brief Converts a Qty to its string representation.
/// @param qty The Qty to convert.
/// @return String representation, or "INVALID".
inline auto QtyToString(Qty qty) -> std::string {
    if (qty == Qty_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(qty);
}

/// @brief Type alias for Priority in the FIFO queue.
using Priority = uint64_t;

/// @brief Invalid value marker for Priority.
constexpr auto Priority_INVALID = std::numeric_limits<Priority>::max();

/// @brief Converts a Priority to its string representation.
/// @param priority The Priority to convert.
/// @return String representation, or "INVALID".
inline auto PriorityToString(Priority priority) -> std::string {
    if (priority == Priority_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(priority);
}

/// @brief Enum representing the side of an order.
enum class Side : int8_t { INVALID = 0, BUY = 1, SELL = -1, MAX = 2 };

/// @brief Converts a Side enum to its string representation.
/// @param side The Side to convert.
/// @return String representation.
inline auto SideToString(Side side) -> std::string {
    switch (side) {
        case Side::BUY:
            return "BUY";
        case Side::SELL:
            return "SELL";
        case Side::INVALID:
            return "INVALID";
        case Side::MAX:
            return "MAX";
    }
    return "UNKNOWN";
}

/// @brief Maps a Side to an array index.
/// @param side The Side to convert.
/// @return Index value.
constexpr auto SideToIndex(Side side) noexcept { return static_cast<size_t>(side) + 1; }

#endif  // SOLO_STRATEGY_SRC_UTILITIES_TYPES_H_
