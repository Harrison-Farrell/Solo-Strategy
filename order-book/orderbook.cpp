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
    // check the update is higher than the current bid price
    const auto bid_updated =
        (mBids_by_price && market_update->side_ == Side::BUY &&
         market_update->price_ >= mBids_by_price->mPrice);
    // check the update is lower than the current ask price
    const auto ask_update =
        (mAsks_by_price && market_update->side_ == Side::SELL &&
         market_update->price_ <= mAsks_by_price->mPrice);
}