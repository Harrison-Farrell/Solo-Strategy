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
/// @brief Definitions for market orders, price levels, and best bid/offer
/// views.

#ifndef SOLO_STRATEGY_SRC_MARKET_ORDERS_MARKET_ORDER_H_
#define SOLO_STRATEGY_SRC_MARKET_ORDERS_MARKET_ORDER_H_

#include <array>
#include <unordered_map>

#include "utilities/types.h"

class OrderBook;

/// @brief Represents a single market order in the order book.
class MarketOrder {
    friend class OrderBook;

   private:
    /// @brief Order identifier.
    OrderId m_orderId = OrderId_INVALID;
    /// @brief Side of the order (buy/sell).
    Side m_side = Side::INVALID;
    /// @brief Price of the order.
    Price m_price = Price_INVALID;
    /// @brief Quantity of the order.
    Qty m_qty = Qty_INVALID;
    /// @brief Priority of the order.
    Priority m_priority = Priority_INVALID;

    /// @brief Pointer to the previous order in the linked list.
    MarketOrder* m_prevOrder = nullptr;
    /// @brief Pointer to the next order in the linked list.
    MarketOrder* m_nextOrder = nullptr;

   public:
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
    MarketOrder(OrderId order_id, Side side, Price price, Qty qty,
                Priority priority, MarketOrder* prev_order,
                MarketOrder* next_order) noexcept;

    /// @brief Returns a string representation of the MarketOrder.
    /// @return String representation.
    auto ToString() const -> std::string;
};

using OrderArray = std::unordered_map<OrderId, MarketOrder*>;

/// @brief Represents all market orders at a specific price level.
class MarketOrderAtPrice {
    friend class OrderBook;

   private:
    /// @brief Side of the orders at this price.
    Side m_side = Side::INVALID;
    /// @brief Price level.
    Price m_price = Price_INVALID;

    /// @brief Pointer to the first market order at this price.
    MarketOrder* m_firstMarketOrder = nullptr;

    /// @brief Pointer to the previous price entry in the linked list.
    MarketOrderAtPrice* m_prevEntry = nullptr;
    /// @brief Pointer to the next price entry in the linked list.
    MarketOrderAtPrice* m_nextEntry = nullptr;

   public:
    /// @brief Default constructor.
    MarketOrderAtPrice() = default;

    /// @brief Constructs a MarketOrderAtPrice with all fields specified.
    /// @param side Side of the orders.
    /// @param price Price level.
    /// @param first_market_order Pointer to first market order.
    /// @param prev_entry Pointer to previous price entry.
    /// @param next_entry Pointer to next price entry.
    MarketOrderAtPrice(Side side, Price price, MarketOrder* first_market_order,
                       MarketOrderAtPrice* prev_entry,
                       MarketOrderAtPrice* next_entry) noexcept;

    /// @brief Returns a string representation of the MarketOrderAtPrice.
    /// @return String representation.
    auto ToString() const -> std::string;
};

using OrdersAtPriceArray = std::unordered_map<Price, MarketOrderAtPrice*>;
/// @brief Represents a single price level in the depth view.
struct DepthLevel {
    Price price;
    Qty qty;
};

/// @brief Represents the best bid and offer in the order book.
class BestBidOffer {
    friend class OrderBook;

   private:
    /// @brief Best bid price.
    Price m_bidPrice = Price_INVALID;
    /// @brief Best ask price.
    Price m_askPrice = Price_INVALID;

    /// @brief Quantity at best bid.
    Qty m_bidQty = Qty_INVALID;
    /// @brief Quantity at best ask.
    Qty m_askQty = Qty_INVALID;

   public:
    /// @brief Returns a string representation of the BestBidOffer.
    /// @return String representation.
    auto ToString() const -> std::string;
};

#endif  // SOLO_STRATEGY_SRC_MARKET_ORDERS_MARKET_ORDER_H_