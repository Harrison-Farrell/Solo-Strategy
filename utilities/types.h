#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <sstream>

#include "macros.h"

// Constants used across the ecosystem to represent upper bounds on various
// containers. Trading instruments / TickerIds from [0, ME_MAX_TICKERS].
constexpr size_t ME_MAX_TICKERS = 8;

// Maximum size of lock free queues used to transfer client requests,
// client responses and market updates between components.
constexpr size_t ME_MAX_CLIENT_UPDATES = 256 * 1024;
constexpr size_t ME_MAX_MARKET_UPDATES = 256 * 1024;

// Maximum trading clients.
constexpr size_t ME_MAX_NUM_CLIENTS = 256;

// Maximum number of orders per trading client.
constexpr size_t ME_MAX_ORDER_IDS = 1024 * 1024;

// Maximum price level depth in the order books.
constexpr size_t ME_MAX_PRICE_LEVELS = 256;

typedef uint64_t OrderId;
constexpr auto OrderId_INVALID = std::numeric_limits<OrderId>::max();

inline auto orderIdToString(OrderId order_id) -> std::string {
    if (order_id == OrderId_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(order_id);
}

typedef uint32_t TickerId;
constexpr auto TickerId_INVALID = std::numeric_limits<TickerId>::max();

inline auto tickerIdToString(TickerId ticker_id) -> std::string {
    if (ticker_id == TickerId_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(ticker_id);
}

typedef uint32_t ClientId;
constexpr auto ClientId_INVALID = std::numeric_limits<ClientId>::max();

inline auto clientIdToString(ClientId client_id) -> std::string {
    if (client_id == ClientId_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(client_id);
}

typedef int64_t Price;
constexpr auto Price_INVALID = std::numeric_limits<Price>::max();

inline auto priceToString(Price price) -> std::string {
    if (price == Price_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(price);
}

typedef uint32_t Qty;
constexpr auto Qty_INVALID = std::numeric_limits<Qty>::max();

inline auto qtyToString(Qty qty) -> std::string {
    if (qty == Qty_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(qty);
}

/// Priority represents position in the FIFO queue for all orders with the same
/// side and price attributes.
typedef uint64_t Priority;
constexpr auto Priority_INVALID = std::numeric_limits<Priority>::max();

inline auto priorityToString(Priority priority) -> std::string {
    if (priority == Priority_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(priority);
}

enum class Side : int8_t { INVALID = 0, BUY = 1, SELL = -1, MAX = 2 };

inline auto sideToString(Side side) -> std::string {
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

/// Convert Side to an index which can be used to index into a std::array.
inline constexpr auto sideToIndex(Side side) noexcept {
    return static_cast<size_t>(side) + 1;
}

/// Convert Side::BUY=1 and Side::SELL=-1.
inline constexpr auto sideToValue(Side side) noexcept {
    return static_cast<int>(side);
}
