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

#include "market-orders/market_order.h"
#include "market-orders/market_update.h"
#include "memory-pool/memory_pool.h"
#include "utilities/macros.h"
#include "utilities/types.h"

/// @brief Represents the limit order book for a single trading instrument.
class MarketOrderBook final {
   public:
    /// @brief Constructs a MarketOrderBook for the given ticker.
    /// @param ticker_id The ticker identifier for the instrument.
    MarketOrderBook(TickerId ticker_id);

    /// Deleted default, copy & move constructors and assignment-operators.
    MarketOrderBook() = delete;
    MarketOrderBook(const MarketOrderBook&) = delete;
    MarketOrderBook(const MarketOrderBook&&) = delete;
    MarketOrderBook& operator=(const MarketOrderBook&) = delete;
    MarketOrderBook& operator=(const MarketOrderBook&&) = delete;

    /// @brief Destructor for MarketOrderBook.
    ~MarketOrderBook();

    /// @brief Processes a market update and updates the order book accordingly.
    /// Handles ADD, MODIFY, CANCEL, and CLEAR operations.
    /// @param market_update Pointer to the market update message.
    auto OnMarketUpdate(const MEMarketUpdate* market_update) noexcept -> void;

    /// @brief Updates the BestBidOffer view based on the current state of the
    /// book.
    /// @param update_bid Flag to update the bid side.
    /// @param update_ask Flag to update the ask side.
    auto UpdateBestBidOffer(bool update_bid, bool update_ask) noexcept {
        if (update_bid) {
            if (static_cast<bool>(m_bidsByPrice)) {
                m_bestBidOffer.m_bidPrice = m_bidsByPrice->m_price;
                m_bestBidOffer.m_bidQty =
                    m_bidsByPrice->m_firstMarketOrder->m_qty;
                for (auto* order =
                         m_bidsByPrice->m_firstMarketOrder->m_nextOrder;
                     order != m_bidsByPrice->m_firstMarketOrder;
                     order = order->m_nextOrder) {
                    m_bestBidOffer.m_bidQty += order->m_qty;
                }
            } else {
                // There is no head the the m_bidsByPrice is nullptr
                m_bestBidOffer.m_bidPrice = Price_INVALID;
                m_bestBidOffer.m_askQty = Qty_INVALID;
            }
        }

        if (update_ask) {
            if (static_cast<bool>(m_asksByPrice)) {
                m_bestBidOffer.m_askPrice = m_asksByPrice->m_price;
                m_bestBidOffer.m_askQty =
                    m_asksByPrice->m_firstMarketOrder->m_qty;
                for (auto* order =
                         m_asksByPrice->m_firstMarketOrder->m_nextOrder;
                     order != m_asksByPrice->m_firstMarketOrder;
                     order = order->m_nextOrder) {
                    m_bestBidOffer.m_askQty += order->m_qty;
                }
            } else {
                // There is no head the the m_asksByPrice is nullptr
                m_bestBidOffer.m_askPrice = Price_INVALID;
                m_bestBidOffer.m_askQty = Qty_INVALID;
            }
        }
    }

    /// @brief Returns the current BestBidOffer view.
    /// @return Pointer to the BestBidOffer structure.
    auto GetBestBidOffer() const noexcept -> const BestBidOffer* {
        return &m_bestBidOffer;
    }

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
    auto AddOrdersAtPrice(MarketOrderAtPrice* new_orders_at_price) noexcept {
        m_priceOrdersAtPrice.at(PriceToIndex(new_orders_at_price->m_price)) =
            new_orders_at_price;

        const auto best_orders_by_price =
            (new_orders_at_price->m_side == Side::BUY ? m_bidsByPrice
                                                      : m_asksByPrice);
        if (!best_orders_by_price) [[unlikely]] {
            (new_orders_at_price->m_side == Side::BUY ? m_bidsByPrice
                                                      : m_asksByPrice) =
                new_orders_at_price;
            new_orders_at_price->m_prevEntry =
                new_orders_at_price->m_nextEntry = new_orders_at_price;
        } else {
            auto target = best_orders_by_price;
            bool add_after =
                ((new_orders_at_price->m_side == Side::SELL &&
                  new_orders_at_price->m_price > target->m_price) ||
                 (new_orders_at_price->m_side == Side::BUY &&
                  new_orders_at_price->m_price < target->m_price));
            if (add_after) {
                target = target->m_nextEntry;
                add_after = ((new_orders_at_price->m_side == Side::SELL &&
                              new_orders_at_price->m_price > target->m_price) ||
                             (new_orders_at_price->m_side == Side::BUY &&
                              new_orders_at_price->m_price < target->m_price));
            }
            while (add_after && target != best_orders_by_price) {
                add_after = ((new_orders_at_price->m_side == Side::SELL &&
                              new_orders_at_price->m_price > target->m_price) ||
                             (new_orders_at_price->m_side == Side::BUY &&
                              new_orders_at_price->m_price < target->m_price));
                if (add_after) {
                    target = target->m_nextEntry;
                }
            }

            if (add_after) {  // add new_orders_at_price after target.
                if (target == best_orders_by_price) {
                    target = best_orders_by_price->m_prevEntry;
                }
                new_orders_at_price->m_prevEntry = target;
                target->m_nextEntry->m_prevEntry = new_orders_at_price;
                new_orders_at_price->m_nextEntry = target->m_nextEntry;
                target->m_nextEntry = new_orders_at_price;
            } else {  // add new_orders_at_price before target.
                new_orders_at_price->m_prevEntry = target->m_prevEntry;
                new_orders_at_price->m_nextEntry = target;
                target->m_prevEntry->m_nextEntry = new_orders_at_price;
                target->m_prevEntry = new_orders_at_price;

                if ((new_orders_at_price->m_side == Side::BUY &&
                     new_orders_at_price->m_price >
                         best_orders_by_price->m_price) ||
                    (new_orders_at_price->m_side == Side::SELL &&
                     new_orders_at_price->m_price <
                         best_orders_by_price->m_price)) {
                    target->m_nextEntry =
                        (target->m_nextEntry == best_orders_by_price
                             ? new_orders_at_price
                             : target->m_nextEntry);
                    (new_orders_at_price->m_side == Side::BUY ? m_bidsByPrice
                                                              : m_asksByPrice) =
                        new_orders_at_price;
                }
            }
        }
    }

    /// @brief Adds an order to the book, creating a new price level if
    /// necessary.
    /// @param order Pointer to the MarketOrder to add.
    auto addOrder(MarketOrder* order) noexcept -> void {
        // Look up the existing price level for this order's price
        const auto orders_at_price = GetOrdersAtPrice(order->m_price);

        if (!orders_at_price) {
            // No existing price level, so create a new one
            // Initialize the order as a circular doubly-linked list with itself
            order->m_nextOrder = order->m_prevOrder = order;

            // Allocate a new MarketOrderAtPrice container for this price level
            auto new_orders_at_price = m_ordersAtPricePool.Allocate(
                order->m_side, order->m_price, order, nullptr, nullptr);
            // Add the new price level to the price-level linked list
            AddOrdersAtPrice(new_orders_at_price);
        } else {
            // Price level already exists, append order to the FIFO queue at
            // this price
            auto first_order =
                (orders_at_price ? orders_at_price->m_firstMarketOrder
                                 : nullptr);

            // Insert the order at the end of the circular doubly-linked list
            first_order->m_prevOrder->m_nextOrder = order;
            order->m_prevOrder = first_order->m_prevOrder;
            order->m_nextOrder = first_order;
            first_order->m_prevOrder = order;
        }

        // Track the order in the order ID array for fast lookup
        m_orderIdToOrder.at(order->m_orderId) = order;
    }

    /// Remove the MarketOrderAtPrice from the containers - the hash map and
    /// the doubly linked list of price levels.
    auto removeOrdersAtPrice(Side side, Price price) noexcept {
        const auto best_orders_by_price =
            (side == Side::BUY ? m_bidsByPrice : m_asksByPrice);
        auto orders_at_price = GetOrdersAtPrice(price);

        if (orders_at_price->m_nextEntry == orders_at_price)
            [[unlikely]] {  // empty side of book.
            (side == Side::BUY ? m_bidsByPrice : m_asksByPrice) = nullptr;
        } else {
            orders_at_price->m_prevEntry->m_nextEntry =
                orders_at_price->m_nextEntry;
            orders_at_price->m_nextEntry->m_prevEntry =
                orders_at_price->m_prevEntry;

            if (orders_at_price == best_orders_by_price) {
                (side == Side::BUY ? m_bidsByPrice : m_asksByPrice) =
                    orders_at_price->m_nextEntry;
            }

            orders_at_price->m_prevEntry = orders_at_price->m_nextEntry =
                nullptr;
        }

        m_priceOrdersAtPrice.at(PriceToIndex(price)) = nullptr;

        m_ordersAtPricePool.Deallocate(orders_at_price);
    }

    auto removeOrder(MarketOrder* order) noexcept -> void {
        auto orders_at_price = GetOrdersAtPrice(order->m_price);

        if (order->m_prevOrder == order) {  // only one element.
            removeOrdersAtPrice(order->m_side, order->m_price);
        } else {  // remove the link.
            const auto order_before = order->m_prevOrder;
            const auto order_after = order->m_nextOrder;
            order_before->m_nextOrder = order_after;
            order_after->m_prevOrder = order_before;

            if (orders_at_price->m_firstMarketOrder == order) {
                orders_at_price->m_firstMarketOrder = order_after;
            }

            order->m_prevOrder = order->m_nextOrder = nullptr;
        }

        m_orderIdToOrder.at(order->m_orderId) = nullptr;
        m_orderPool.Deallocate(order);
    }
};

/// \typedef MarketOrderBookHashMap
/// \brief Hash map from TickerId to MarketOrderBook.
typedef std::array<MarketOrderBook*, ME_MAX_TICKERS> MarketOrderBookHashMap;

#endif  // SOLO_STRATEGY_SRC_ORDER_BOOK_ORDER_BOOK_H_