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

/// @file order_book.h
/// @brief Core LOB (Limit Order Book) implementation for the trading system.

#ifndef SOLO_STRATEGY_SRC_ORDER_BOOK_ORDER_BOOK_H_
#define SOLO_STRATEGY_SRC_ORDER_BOOK_ORDER_BOOK_H_

// system includes
#include <unordered_map>

// local includes
#include "market-orders/market_order.h"
#include "market-orders/market_update.h"
#include "memory-pool/memory_pool.h"
#include "utilities/types.h"

// 3rd party libraries
#include "quill/Logger.h"

/// @brief Represents the limit order book for a single trading instrument.
class OrderBook final {
   public:
    /// @brief Constructs a OrderBook for the given ticker.
    /// @param ticker_id The ticker identifier for the instrument.
    OrderBook(TickerId ticker_id);

    /// Deleted default, copy & move constructors and assignment-operators.
    OrderBook() = delete;
    OrderBook(const OrderBook&) = delete;
    OrderBook(const OrderBook&&) = delete;
    OrderBook& operator=(const OrderBook&) = delete;
    OrderBook& operator=(const OrderBook&&) = delete;

    /// @brief Destructor for OrderBook.
    ~OrderBook();

    /// @brief Processes a market update and updates the order book accordingly.
    /// Handles ADD, MODIFY, CANCEL, and CLEAR operations.
    /// @param market_update Pointer to the market update message.
    auto OnMarketUpdate(const MarketUpdate* market_update) noexcept -> void;

    /// @brief Updates the BestBidOffer view based on the current state of the
    /// book.
    /// @param update_bid Flag to update the bid side.
    /// @param update_ask Flag to update the ask side.
    inline auto UpdateBestBidOffer(bool update_bid, bool update_ask) noexcept;

    /// @brief Returns the current BestBidOffer view.
    /// @return Pointer to the BestBidOffer structure.
    auto GetBestBidOffer() const noexcept -> const BestBidOffer*;

   private:
    /// @brief The ticker identifier for the instrument.
    const TickerId m_tickerId;
    /// @brief Array of orders indexed by their order id.
    OrderArray m_orderIdToOrder;
    /// @brief Memory pool for allocating MarketOrderAtPrice objects.
    MemoryPool<MarketOrderAtPrice> m_ordersAtPricePool;
    /// @brief Head of the bids linked list (highest price first).
    MarketOrderAtPrice* m_bidsByPrice = nullptr;
    /// @brief Head of the asks linked list (lowest price first).
    MarketOrderAtPrice* m_asksByPrice = nullptr;
    /// @brief Array of price levels indexed by hashed price.
    OrdersAtPriceArray m_priceOrdersAtPrice;
    /// @brief Memory pool for allocating MarketOrder objects.
    MemoryPool<MarketOrder> m_orderPool;
    /// @brief current best bid and offer for the book.
    BestBidOffer m_bestBidOffer;
    /// @brief Last update time string.
    std::string m_timeString;
    /// @brief Logger for this order book.
    quill::Logger* m_logger = nullptr;

    /// @brief Maps a price to an index for constant-time lookup.
    /// @param price The price value to map.
    /// @return Index in the range [0, ME_MAX_PRICE_LEVELS).
    static auto PriceToIndex(Price price) noexcept {
        return price % ME_MAX_PRICE_LEVELS;
    }

    /// @brief Retrieves the price level container for a given price.
    /// @param price The price to look up.
    /// @return Pointer to MarketOrderAtPrice, or nullptr if none exists.
    auto GetOrdersAtPrice(Price price) const noexcept -> MarketOrderAtPrice* {
        return m_priceOrdersAtPrice.at(PriceToIndex(price));
    }

    /// @brief Adds a new price level to the sorted linked list.
    /// @param new_orders_at_price Pointer to the new price level container.
    /// Add a new MarketOrderAtPrice at the correct price into the containers -
    /// the hash map and the doubly linked list of price levels.
    inline auto AddOrdersAtPrice(
        MarketOrderAtPrice* new_orders_at_price) noexcept;

    /// @brief Adds an order to the book, creating a new price level if
    /// necessary.
    /// @param order Pointer to the MarketOrder to add.
    inline auto AddOrder(MarketOrder* order) noexcept -> void;

    /// Remove the MarketOrderAtPrice from the containers - the hash map and
    /// the doubly linked list of price levels.
    inline auto RemoveOrdersAtPrice(Side side, Price price) noexcept;

    /// Remove an order from the book.
    /// @param order Pointer to the MarketOrder to remove.
    inline auto RemoveOrder(MarketOrder* order) noexcept -> void;
};

/// \typedef OrderBookHashMap
/// \brief Hash map from TickerId to OrderBook.
using OrderBookHashMap = std::array<OrderBook*, ME_MAX_TICKERS>;

#endif  // SOLO_STRATEGY_SRC_ORDER_BOOK_ORDER_BOOK_H_