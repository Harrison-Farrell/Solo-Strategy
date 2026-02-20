#ifndef SOLO_STRATEGY_SRC_UTILITIES_TYPES_H_
#define SOLO_STRATEGY_SRC_UTILITIES_TYPES_H_

#include <array>
#include <cstdint>
#include <limits>
#include <sstream>

#include "macros.h"

/** @brief Maximum number of trading instruments/tickers. */
constexpr size_t ME_MAX_TICKERS = 8;

/** @brief Maximum size of lock-free queues for client updates. */
constexpr size_t ME_MAX_CLIENT_UPDATES = 256 * 1024;

/** @brief Maximum size of lock-free queues for market updates. */
constexpr size_t ME_MAX_MARKET_UPDATES = 256 * 1024;

/** @brief Maximum number of trading clients. */
constexpr size_t ME_MAX_NUM_CLIENTS = 256;

/** @brief Maximum number of orders per trading client. */
constexpr size_t ME_MAX_ORDER_IDS = 1024 * 1024;

/** @brief Maximum price level depth in the order books. */
constexpr size_t ME_MAX_PRICE_LEVELS = 256;

/** @brief Type alias for OrderId. */
typedef uint64_t OrderId;

/** @brief Invalid value marker for OrderId. */
constexpr auto OrderId_INVALID = std::numeric_limits<OrderId>::max();

/**
 * @brief Converts an OrderId to its string representation.
 * @param order_id The OrderId to convert.
 * @return String representation, or "INVALID".
 */
inline auto orderIdToString(OrderId order_id) -> std::string {
    if (order_id == OrderId_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(order_id);
}

/** @brief Type alias for TickerId. */
typedef uint32_t TickerId;

/** @brief Invalid value marker for TickerId. */
constexpr auto TickerId_INVALID = std::numeric_limits<TickerId>::max();

/**
 * @brief Converts a TickerId to its string representation.
 * @param ticker_id The TickerId to convert.
 * @return String representation, or "INVALID".
 */
inline auto tickerIdToString(TickerId ticker_id) -> std::string {
    if (ticker_id == TickerId_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(ticker_id);
}

/** @brief Type alias for ClientId. */
typedef uint32_t ClientId;

/** @brief Invalid value marker for ClientId. */
constexpr auto ClientId_INVALID = std::numeric_limits<ClientId>::max();

/**
 * @brief Converts a ClientId to its string representation.
 * @param client_id The ClientId to convert.
 * @return String representation, or "INVALID".
 */
inline auto clientIdToString(ClientId client_id) -> std::string {
    if (client_id == ClientId_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(client_id);
}

/** @brief Type alias for Price. Signed as negative prices are possible in some contexts. */
typedef int64_t Price;

/** @brief Invalid value marker for Price. */
constexpr auto Price_INVALID = std::numeric_limits<Price>::max();

/**
 * @brief Converts a Price to its string representation.
 * @param price The Price to convert.
 * @return String representation, or "INVALID".
 */
inline auto priceToString(Price price) -> std::string {
    if (price == Price_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(price);
}

/** @brief Type alias for Quantity. */
typedef uint32_t Qty;

/** @brief Invalid value marker for Qty. */
constexpr auto Qty_INVALID = std::numeric_limits<Qty>::max();

/**
 * @brief Converts a Qty to its string representation.
 * @param qty The Qty to convert.
 * @return String representation, or "INVALID".
 */
inline auto qtyToString(Qty qty) -> std::string {
    if (qty == Qty_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(qty);
}

/** @brief Type alias for Priority in the FIFO queue. */
typedef uint64_t Priority;

/** @brief Invalid value marker for Priority. */
constexpr auto Priority_INVALID = std::numeric_limits<Priority>::max();

/**
 * @brief Converts a Priority to its string representation.
 * @param priority The Priority to convert.
 * @return String representation, or "INVALID".
 */
inline auto priorityToString(Priority priority) -> std::string {
    if (priority == Priority_INVALID) [[unlikely]] {
        return "INVALID";
    }

    return std::to_string(priority);
}

/** @brief Enum representing the side of an order. */
enum class Side : int8_t { INVALID = 0, BUY = 1, SELL = -1, MAX = 2 };

/**
 * @brief Converts a Side enum to its string representation.
 * @param side The Side to convert.
 * @return String representation.
 */
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

/**
 * @brief Maps a Side to an array index.
 * @param side The Side to convert.
 * @return Index value.
 */
inline constexpr auto sideToIndex(Side side) noexcept { return static_cast<size_t>(side) + 1; }

#endif  // SOLO_STRATEGY_SRC_UTILITIES_TYPES_H_
