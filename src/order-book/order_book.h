/*
 * --------------------------------------------------------------------------
 * Author:      Harrison Farrell
 * Project:     Solo-Strategy Trading System
 * Copyright:   (c) 2026 Harrison Farrell. All Rights Reserved.
 *
 * Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
 * This program is distributed WITHOUT ANY WARRANTY; without even the 
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See <https://www.gnu.org/licenses/agpl-3.0.html> for full details.
 * --------------------------------------------------------------------------
 */

/**
 * @file order_book.h
 * @brief Core LOB (Limit Order Book) implementation for the trading system.
 */

#ifndef SOLO_STRATEGY_SRC_ORDER_BOOK_ORDER_BOOK_H_
#define SOLO_STRATEGY_SRC_ORDER_BOOK_ORDER_BOOK_H_

#include "market-orders/market_order.h"
#include "market-orders/market_update.h"
#include "memory-pool/memory_pool.h"
#include "utilities/macros.h"
#include "utilities/types.h"

/**
 * @brief Represents the limit order book for a single trading instrument.
 */
class MarketOrderBook final {
   public:
    /**
     * @brief Constructs a MarketOrderBook for the given ticker.
     * @param ticker_id The ticker identifier for the instrument.
     */
    MarketOrderBook(TickerId ticker_id);

    /**
     * @brief Destructor for MarketOrderBook.
     */
    ~MarketOrderBook();

    /**
     * @brief Processes a market update and updates the order book accordingly.
     * Handles ADD, MODIFY, CANCEL, and CLEAR operations.
     * @param market_update Pointer to the market update message.
     */
    auto onMarketUpdate(const MEMarketUpdate *market_update) noexcept -> void;

    /**
     * @brief Updates the BestBidOffer view based on the current state of the book.
     * @param update_bid Flag to update the bid side.
     * @param update_ask Flag to update the ask side.
     */
    auto updateBestBidOffer(bool update_bid, bool update_ask) noexcept {
        if (update_bid) {
            if (mBids_by_price) {
                mBest_bid_offer.mBid_price = mBids_by_price->mPrice;
                mBest_bid_offer.mBid_qty = mBids_by_price->mFirst_market_order->mQty;
                for (auto order = mBids_by_price->mFirst_market_order->mNext_order;
                     order != mBids_by_price->mFirst_market_order; order = order->mNext_order)
                    mBest_bid_offer.mBid_qty += order->mQty;
            } else {
                // There is no head the the mBids_by_price is nullptr
                mBest_bid_offer.mBid_price = Price_INVALID;
                mBest_bid_offer.mAsk_qty = Qty_INVALID;
            }
        }

        if (update_ask) {
            if (mAsks_by_price) {
                mBest_bid_offer.mAsk_price = mAsks_by_price->mPrice;
                mBest_bid_offer.mAsk_qty = mAsks_by_price->mFirst_market_order->mQty;
                for (auto order = mAsks_by_price->mFirst_market_order->mNext_order;
                     order != mAsks_by_price->mFirst_market_order; order = order->mNext_order)
                    mBest_bid_offer.mAsk_qty += order->mQty;
            } else {
                // There is no head the the mAsks_by_price is nullptr
                mBest_bid_offer.mAsk_price = Price_INVALID;
                mBest_bid_offer.mAsk_qty = Qty_INVALID;
            }
        }
    }

    /**
     * @brief Returns the current BestBidOffer view.
     * @return Pointer to the BestBidOffer structure.
     */
    auto getBestBidOffer() const noexcept -> const BestBidOffer * { return &mBest_bid_offer; }

   private:
    /** @brief The ticker identifier for the instrument. */
    const TickerId mTicker_id;

    /** @brief Array of orders indexed by their order id. */
    OrderArray mOrder_id_to_order;
    /** @brief Memory pool for allocating MarketOrderAtPrice objects. */
    MemoryPool<MarketOrderAtPrice> mOrders_at_price_pool;
    /** @brief Head of the bids linked list (highest price first). */
    MarketOrderAtPrice *mBids_by_price = nullptr;
    /** @brief Head of the asks linked list (lowest price first). */
    MarketOrderAtPrice *mAsks_by_price = nullptr;
    /** @brief Array of price levels indexed by hashed price. */
    OrdersAtPriceArray mPrice_orders_at_price;
    /** @brief Memory pool for allocating MarketOrder objects. */
    MemoryPool<MarketOrder> mOrder_pool;

    /** @brief current best bid and offer for the book. */
    BestBidOffer mBest_bid_offer;
    /** @brief Last update time string. */
    std::string mtime_str;

   private:
    /**
     * @brief Maps a price to an index for constant-time lookup.
     * @param price The price value to map.
     * @return Index in the range [0, ME_MAX_PRICE_LEVELS).
     */
    inline auto priceToIndex(Price price) const noexcept { return price % ME_MAX_PRICE_LEVELS; }

    /**
     * @brief Retrieves the price level container for a given price.
     * @param price The price to look up.
     * @return Pointer to MarketOrderAtPrice, or nullptr if none exists.
     */
    inline auto getOrdersAtPrice(Price price) const noexcept -> MarketOrderAtPrice * {
        return mPrice_orders_at_price.at(priceToIndex(price));
    }

    /**
     * @brief Adds a new price level to the sorted linked list.
     * @param new_orders_at_price Pointer to the new price level container.
     */
    auto addOrdersAtPrice(MarketOrderAtPrice *new_orders_at_price) noexcept {
        // Map the price to an index and store the new price level in the hash
        // map
        mPrice_orders_at_price.at(priceToIndex(new_orders_at_price->mPrice)) = new_orders_at_price;

        // Get the current best price level (head of the linked list) for this
        // side
        const auto best_orders_by_price =
            (new_orders_at_price->mSide == Side::BUY ? mBids_by_price : mAsks_by_price);

        // If this is the first price level, initialize the circular linked list
        if (!best_orders_by_price) [[unlikely]] {
            (new_orders_at_price->mSide == Side::BUY ? mBids_by_price : mAsks_by_price) =
                new_orders_at_price;
            // Point to itself as it's the only element in the circular list
            new_orders_at_price->mPrev_entry = new_orders_at_price->mNext_entry =
                new_orders_at_price;
        } else {
            // Start with the best price level
            auto target = best_orders_by_price;

            // Determine if the new price should be inserted after the best
            // price For SELL orders: insert after if new price > best price For
            // BUY orders: insert after if new price < best price
            bool add_after = ((new_orders_at_price->mSide == Side::SELL &&
                               new_orders_at_price->mPrice > target->mPrice) ||
                              (new_orders_at_price->mSide == Side::BUY &&
                               new_orders_at_price->mPrice < target->mPrice));

            // Move to the next entry if add_after is true
            if (add_after) {
                target = target->mNext_entry;
                // Re-check the condition with the next entry
                add_after = ((new_orders_at_price->mSide == Side::SELL &&
                              new_orders_at_price->mPrice > target->mPrice) ||
                             (new_orders_at_price->mSide == Side::BUY &&
                              new_orders_at_price->mPrice < target->mPrice));
            }

            // Traverse the linked list to find the correct insertion position
            while (add_after && target != best_orders_by_price) {
                add_after = ((new_orders_at_price->mSide == Side::SELL &&
                              new_orders_at_price->mPrice > target->mPrice) ||
                             (new_orders_at_price->mSide == Side::BUY &&
                              new_orders_at_price->mPrice < target->mPrice));
                if (add_after) target = target->mNext_entry;
            }

            if (add_after) {
                // Insert new_orders_at_price after the target position
                // If target is the best price, insert at the end of the list
                if (target == best_orders_by_price) {
                    target = best_orders_by_price->mPrev_entry;
                }
                // Link the new entry into the list after target
                new_orders_at_price->mPrev_entry = target;
                target->mNext_entry->mPrev_entry = new_orders_at_price;
                new_orders_at_price->mNext_entry = target->mNext_entry;
                target->mNext_entry = new_orders_at_price;
            } else {
                // Insert new_orders_at_price before the target position
                new_orders_at_price->mPrev_entry = target->mPrev_entry;
                new_orders_at_price->mNext_entry = target;
                target->mPrev_entry->mNext_entry = new_orders_at_price;
                target->mPrev_entry = new_orders_at_price;

                // Update the best price level if the new price is better
                // For BUY: better price is higher
                // For SELL: better price is lower
                if ((new_orders_at_price->mSide == Side::BUY &&
                     new_orders_at_price->mPrice > best_orders_by_price->mPrice) ||
                    (new_orders_at_price->mSide == Side::SELL &&
                     new_orders_at_price->mPrice < best_orders_by_price->mPrice)) {
                    target->mNext_entry =
                        (target->mNext_entry == best_orders_by_price ? new_orders_at_price
                                                                     : target->mNext_entry);
                    // Update the head pointer to point to the new best price
                    // level
                    (new_orders_at_price->mSide == Side::BUY ? mBids_by_price : mAsks_by_price) =
                        new_orders_at_price;
                }
            }
        }
    }

    /**
     * @brief Adds an order to the book, creating a new price level if necessary.
     * @param order Pointer to the MarketOrder to add.
     */
    auto addOrder(MarketOrder *order) noexcept -> void {
        // Look up the existing price level for this order's price
        const auto orders_at_price = getOrdersAtPrice(order->mPrice);

        if (!orders_at_price) {
            // No existing price level, so create a new one
            // Initialize the order as a circular doubly-linked list with itself
            order->mNext_order = order->mPrev_order = order;

            // Allocate a new MarketOrderAtPrice container for this price level
            auto new_orders_at_price = mOrders_at_price_pool.allocate(order->mSide, order->mPrice,
                                                                      order, nullptr, nullptr);
            // Add the new price level to the price-level linked list
            addOrdersAtPrice(new_orders_at_price);
        } else {
            // Price level already exists, append order to the FIFO queue at
            // this price
            auto first_order = (orders_at_price ? orders_at_price->mFirst_market_order : nullptr);

            // Insert the order at the end of the circular doubly-linked list
            first_order->mPrev_order->mNext_order = order;
            order->mPrev_order = first_order->mPrev_order;
            order->mNext_order = first_order;
            first_order->mPrev_order = order;
        }

        // Track the order in the order ID array for fast lookup
        mOrder_id_to_order.at(order->mOrder_id) = order;
    }

    /// Remove the MarketOrdersAtPrice from the containers - the hash map and
    /// the doubly linked list of price levels.
    auto removeOrdersAtPrice(Side side, Price price) noexcept {}

    auto removeOrder(MarketOrder *order) noexcept -> void {}
};

/// \typedef MarketOrderBookHashMap
/// \brief Hash map from TickerId to MarketOrderBook.
typedef std::array<MarketOrderBook *, ME_MAX_TICKERS> MarketOrderBookHashMap;

#endif  // SOLO_STRATEGY_SRC_ORDER_BOOK_ORDER_BOOK_H_