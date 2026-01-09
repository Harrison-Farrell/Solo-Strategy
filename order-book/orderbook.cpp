#include "orderbook.h"

MarketOrderBook::MarketOrderBook(TickerId ticker_id)
    : mTicker_id(ticker_id),
      mOrders_at_price_pool(ME_MAX_PRICE_LEVELS),
      mOrder_pool(ME_MAX_ORDER_IDS) {};

MarketOrderBook::~MarketOrderBook() {
    // reset the internal data members
    mBids_by_price = nullptr;
    mAsks_by_price = nullptr;
    mOrder_id_to_oder.fill(nullptr);
}

auto MarketOrderBook::onMarketUpdate(
    const MEMarketUpdate *market_update) noexcept -> void {
    // Check if the bid price level was updated by comparing side and price
    const auto bid_updated =
        (mBids_by_price && market_update->side == Side::BUY &&
         market_update->price >= mBids_by_price->mPrice);
    // Check if the ask price level was updated by comparing side and price
    const auto ask_updated =
        (mAsks_by_price && market_update->side == Side::SELL &&
         market_update->price <= mAsks_by_price->mPrice);

    // Process the market update based on its type
    switch (market_update->type) {
        case MarketUpdateType::ADD: {
            // Allocate a new order from the memory pool with update details
            auto order = mOrder_pool.allocate(
                market_update->order_id, market_update->side,
                market_update->price, market_update->qty,
                market_update->priority, nullptr, nullptr);
            // Add the newly created order to the order book
            addOrder(order);
        } break;
        case MarketUpdateType::MODIFY: {
            // Retrieve the existing order by its ID
            auto order = mOrder_id_to_oder.at(market_update->order_id);
            // Update the order quantity with the new quantity from the update
            order->mOrder_id = market_update->qty;
        } break;
        case MarketUpdateType::CANCEL: {
            // Retrieve the order to be cancelled by its ID
            auto order = mOrder_id_to_oder.at(market_update->order_id);
            // Remove the order from the order book
            removeOrder(order);
        } break;
        case MarketUpdateType::TRADE: {
            // Trade updates are not processed; return early
            return;
        } break;
        case MarketUpdateType::CLEAR: {
            // Clear the full limit order book and deallocate all resources

            // Deallocate all individual orders from the memory pool
            for (auto &order : mOrder_id_to_oder) {
                if (order) mOrder_pool.deallocate(order);
            }
            // Reset the order ID mapping
            mOrder_id_to_oder.fill(nullptr);

            // Deallocate all bid price levels
            if (mBids_by_price) {
                for (auto bid = mBids_by_price->mNext_entry;
                     bid != mBids_by_price; bid = bid->mNext_entry)
                    mOrders_at_price_pool.deallocate(bid);
                mOrders_at_price_pool.deallocate(mBids_by_price);
            }

            // Deallocate all ask price levels
            if (mAsks_by_price) {
                for (auto ask = mAsks_by_price->mNext_entry;
                     ask != mAsks_by_price; ask = ask->mNext_entry)
                    mOrders_at_price_pool.deallocate(ask);
                mOrders_at_price_pool.deallocate(mAsks_by_price);
            }

            // Reset bid and ask pointers
            mBids_by_price = mAsks_by_price = nullptr;
        } break;
        case MarketUpdateType::INVALID:
        case MarketUpdateType::SNAPSHOT_START:
        case MarketUpdateType::SNAPSHOT_END:
            // These update types require no processing
            break;
    }

    // updateBBO(bid_updated, ask_updated);
}