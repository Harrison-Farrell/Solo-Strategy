#pragma once

#include "market-orders/marketorder.h"
#include "market-orders/marketupdate.h"
#include "memory-pool/memorypool.h"
#include "utilities/macros.h"
#include "utilities/types.h"

/// \brief Represents the order book for a single trading instrument.
class MarketOrderBook final {
   public:
    /// \brief Constructs a MarketOrderBook for the given ticker.
    /// \param ticker_id The ticker id for the instrument.
    MarketOrderBook(TickerId ticker_id);

    /// \brief Destructor for MarketOrderBook.
    ~MarketOrderBook();

    /// \brief Processes a market update and updates the limit order book
    /// accordingly.
    ///
    /// This method handles different types of market updates:
    /// - ADD: Allocates and adds a new order to the book.
    /// - MODIFY: Modifies an existing order's quantity.
    /// - CANCEL: Removes an order from the book and deallocates it.
    /// - TRADE: No action is taken (handled elsewhere).
    /// - CLEAR: Clears the entire order book, deallocating all orders and price
    /// levels.
    /// - INVALID, SNAPSHOT_START, SNAPSHOT_END: No action is taken.
    ///
    /// The method also determines if the best bid or ask has been updated and
    /// (optionally) updates the best bid/offer view.
    /// \param market_update Pointer to the market update message.
    /// \return void
    auto onMarketUpdate(const MEMarketUpdate *market_update) noexcept -> void;

    /// \brief Update the BestBidOffer abstraction, the two boolean parameters
    /// represent if the buy or the sekk (or both) sides or both need to be
    /// updated.
    /// \param update_bid flag to update the bid parameters
    /// \param update_ask flag to update the ask parameters
    auto updateBestBidOffer(bool update_bid, bool update_ask) noexcept {
        if (update_bid) {
            if (mBids_by_price) {
                mBest_bid_offer.mBid_price = mBids_by_price->mPrice;
                mBest_bid_offer.mBid_qty =
                    mBids_by_price->mFirst_market_order->mQty;
                for (auto order =
                         mBids_by_price->mFirst_market_order->mNext_order;
                     order != mBids_by_price->mFirst_market_order;
                     order = order->mNext_order)
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
                mBest_bid_offer.mAsk_qty =
                    mAsks_by_price->mFirst_market_order->mQty;
                for (auto order =
                         mAsks_by_price->mFirst_market_order->mNext_order;
                     order != mAsks_by_price->mFirst_market_order;
                     order = order->mNext_order)
                    mBest_bid_offer.mAsk_qty += order->mQty;
            } else {
                // There is no head the the mAsks_by_price is nullptr
                mBest_bid_offer.mAsk_price = Price_INVALID;
                mBest_bid_offer.mAsk_qty = Qty_INVALID;
            }
        }
    }

    auto getBestBidOffer() const noexcept -> const BestBidOffer * {
        return &mBest_bid_offer;
    }

   private:
    /// \brief The ticker id for the instrument.
    const TickerId mTicker_id;

    /// \brief Array of orders indexed by their order id.
    OrderArray mOrder_id_to_oder;
    /// \brief Memory pool to allocate MarketOrderAtPrice objects.
    MemoryPool<MarketOrderAtPrice> mOrders_at_price_pool;
    /// \brief Head of the bids linked list.
    MarketOrderAtPrice *mBids_by_price = nullptr;
    /// \brief Head of the asks linked list.
    MarketOrderAtPrice *mAsks_by_price = nullptr;
    /// \brief Array of orders at a price indexed by their price.
    OrdersAtPriceArray mPrice_orders_at_price;
    /// \brief Memory pool to allocate MarketOrder objects.
    MemoryPool<MarketOrder> mOrder_pool;

    /// \brief Best bid and offer for the order book.
    BestBidOffer mBest_bid_offer;
    /// \brief Time string for the order book.
    std::string mtime_str;

   private:
    /// \brief Maps a price to an index for O(1) lookup in the price-level
    /// array.
    ///
    /// This function uses the modulo operator to map a potentially large or
    /// sparse price value into a valid index within the fixed-size price-level
    /// array. This enables constant-time access to price levels, but assumes
    /// that price collisions (different prices mapping to the same index) are
    /// either handled elsewhere or are not possible in the application's price
    /// domain.
    /// \param price The price to map.
    /// \return Index value in the range [0, ME_MAX_PRICE_LEVELS).
    inline auto priceToIndex(Price price) const noexcept {
        return price % ME_MAX_PRICE_LEVELS;
    }

    /// \brief Fetches the MarketOrdersAtPrice corresponding to a price.
    /// \param price The price to look up.
    /// \return Pointer to MarketOrderAtPrice.
    auto getOrdersAtPrice(Price price) const noexcept -> MarketOrderAtPrice * {
        return mPrice_orders_at_price.at(priceToIndex(price));
    }

    /// \brief Adds a new MarketOrderAtPrice entry to the price-level linked
    /// list.
    ///
    /// This function inserts a new MarketOrderAtPrice node into the circular
    /// doubly-linked list of price levels for either bids or asks, maintaining
    /// the correct order based on price and side (BUY or SELL). If the list is
    /// empty, the new entry becomes the head. Otherwise, it is inserted in the
    /// appropriate position to keep the price levels sorted.
    /// The function also updates the price-to-index mapping for fast lookup.
    /// \param new_orders_at_price Pointer to the MarketOrderAtPrice to add.
    /// \return void
    auto addOrdersAtPrice(MarketOrderAtPrice *new_orders_at_price) noexcept {
        // Map the price to an index and store the new price level in the hash
        // map
        mPrice_orders_at_price.at(priceToIndex(new_orders_at_price->mPrice)) =
            new_orders_at_price;

        // Get the current best price level (head of the linked list) for this
        // side
        const auto best_orders_by_price =
            (new_orders_at_price->mSide == Side::BUY ? mBids_by_price
                                                     : mAsks_by_price);

        // If this is the first price level, initialize the circular linked list
        if (!best_orders_by_price) [[unlikely]] {
            (new_orders_at_price->mSide == Side::BUY ? mBids_by_price
                                                     : mAsks_by_price) =
                new_orders_at_price;
            // Point to itself as it's the only element in the circular list
            new_orders_at_price->mPrev_entry =
                new_orders_at_price->mNext_entry = new_orders_at_price;
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
                     new_orders_at_price->mPrice >
                         best_orders_by_price->mPrice) ||
                    (new_orders_at_price->mSide == Side::SELL &&
                     new_orders_at_price->mPrice <
                         best_orders_by_price->mPrice)) {
                    target->mNext_entry =
                        (target->mNext_entry == best_orders_by_price
                             ? new_orders_at_price
                             : target->mNext_entry);
                    // Update the head pointer to point to the new best price
                    // level
                    (new_orders_at_price->mSide == Side::BUY ? mBids_by_price
                                                             : mAsks_by_price) =
                        new_orders_at_price;
                }
            }
        }
    }

    /// \brief Add a single order at the end of the FIFO queue at the price
    /// level that this order belongs in.
    ///
    /// If there is no existing price level for the order's price, a new
    /// MarketOrderAtPrice is allocated and added. Otherwise, the order is
    /// inserted at the end of the circular doubly-linked list of orders at that
    /// price. The order is also tracked in the order id array for fast lookup.
    /// \param order Pointer to the MarketOrder to add.
    /// \return void
    auto addOrder(MarketOrder *order) noexcept -> void {
        // Look up the existing price level for this order's price
        const auto orders_at_price = getOrdersAtPrice(order->mPrice);

        if (!orders_at_price) {
            // No existing price level, so create a new one
            // Initialize the order as a circular doubly-linked list with itself
            order->mNext_order = order->mPrev_order = order;

            // Allocate a new MarketOrderAtPrice container for this price level
            auto new_orders_at_price = mOrders_at_price_pool.allocate(
                order->mSide, order->mPrice, order, nullptr, nullptr);
            // Add the new price level to the price-level linked list
            addOrdersAtPrice(new_orders_at_price);
        } else {
            // Price level already exists, append order to the FIFO queue at
            // this price
            auto first_order =
                (orders_at_price ? orders_at_price->mFirst_market_order
                                 : nullptr);

            // Insert the order at the end of the circular doubly-linked list
            first_order->mPrev_order->mNext_order = order;
            order->mPrev_order = first_order->mPrev_order;
            order->mNext_order = first_order;
            first_order->mPrev_order = order;
        }

        // Track the order in the order ID array for fast lookup
        mOrder_id_to_oder.at(order->mOrder_id) = order;
    }

    /// Remove the MarketOrdersAtPrice from the containers - the hash map and
    /// the doubly linked list of price levels.
    auto removeOrdersAtPrice(Side side, Price price) noexcept {}

    auto removeOrder(MarketOrder *order) noexcept -> void {}
};

/// \typedef MarketOrderBookHashMap
/// \brief Hash map from TickerId to MarketOrderBook.
typedef std::array<MarketOrderBook *, ME_MAX_TICKERS> MarketOrderBookHashMap;