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

/// @file market_order.h
/// @brief Definitions for market orders, price levels, and best bid/offer views.

#ifndef SOLO_STRATEGY_SRC_MARKET_ORDERS_MARKET_ORDER_H_
#define SOLO_STRATEGY_SRC_MARKET_ORDERS_MARKET_ORDER_H_

#include <string>

#include "utilities/macros.h"
#include "utilities/types.h"

/// @brief Represents a single market order in the order book.
struct MarketOrder {
    /// @brief Order identifier.
    OrderId mOrder_id = OrderId_INVALID;
    /// @brief Side of the order (buy/sell).
    Side mSide = Side::INVALID;
    /// @brief Price of the order.
    Price mPrice = Price_INVALID;
    /// @brief Quantity of the order.
    Qty mQty = Qty_INVALID;
    /// @brief Priority of the order.
    Priority mPriority = Priority_INVALID;

    /// @brief Pointer to the previous order in the linked list.
    MarketOrder* mPrev_order = nullptr;
    /// @brief Pointer to the next order in the linked list.
    MarketOrder* mNext_order = nullptr;

    /// @brief Default constructor. Only needed for use with the Memory Pool.
    MarketOrder() = default;

    /// @brief Constructs a MarketOrder with all fields specified.
    /// @param order_id Order identifier.
    /// @param side Side of the order.
    /// @param price Price of the order.
    /// @param qty Quantity of the order.
    /// @param priority Priority of the order.
    /// @param prev_order Pointer to previous order.
    /// @param next_order Pointer to next order.
    MarketOrder(OrderId order_id, Side side, Price price, Qty qty, Priority priority,
                MarketOrder* prev_order, MarketOrder* next_order) noexcept;

    /// @brief Returns a string representation of the MarketOrder.
    /// @return String representation.
    auto toString() const -> std::string;
};

typedef std::array<MarketOrder*, ME_MAX_ORDER_IDS> OrderArray;

/// @brief Represents all market orders at a specific price level.
struct MarketOrderAtPrice {
    /// @brief Side of the orders at this price.
    Side mSide = Side::INVALID;
    /// @brief Price level.
    Price mPrice = Price_INVALID;

    /// @brief Pointer to the first market order at this price.
    MarketOrder* mFirst_market_order = nullptr;

    /// @brief Pointer to the previous price entry in the linked list.
    MarketOrderAtPrice* mPrev_entry = nullptr;
    /// @brief Pointer to the next price entry in the linked list.
    MarketOrderAtPrice* mNext_entry = nullptr;

    /// @brief Default constructor.
    MarketOrderAtPrice() = default;

    /// @brief Constructs a MarketOrderAtPrice with all fields specified.
    /// @param side Side of the orders.
    /// @param price Price level.
    /// @param first_market_order Pointer to first market order.
    /// @param prev_entry Pointer to previous price entry.
    /// @param next_entry Pointer to next price entry.
    MarketOrderAtPrice(Side side, Price price, MarketOrder* first_market_order,
                       MarketOrderAtPrice* prev_entry, MarketOrderAtPrice* next_entry) noexcept;

    /// @brief Returns a string representation of the MarketOrderAtPrice.
    /// @return String representation.
    auto toString() const -> std::string;
};

typedef std::array<MarketOrderAtPrice*, ME_MAX_PRICE_LEVELS> OrdersAtPriceArray;
/// @brief Represents the best bid and offer in the order book.
struct BestBidOffer {
    /// @brief Best bid price.
    Price mBid_price = Price_INVALID;
    /// @brief Best ask price.
    Price mAsk_price = Price_INVALID;

    /// @brief Quantity at best bid.
    Qty mBid_qty = Qty_INVALID;
    /// @brief Quantity at best ask.
    Qty mAsk_qty = Qty_INVALID;

    /// @brief Returns a string representation of the BestBidOffer.
    /// @return String representation.
    auto toString() const -> std::string;
};

#endif  // SOLO_STRATEGY_SRC_MARKET_ORDERS_MARKET_ORDER_H_