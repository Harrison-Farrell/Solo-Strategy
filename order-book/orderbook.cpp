#include "orderbook.h"

MarketOrderBook::MarketOrderBook(TickerId ticker_id) 
    : mTicker_id(ticker_id),
    mOrders_at_price_pool(ME_MAX_PRICE_LEVELS),
    mOrder_pool(ME_MAX_ORDER_IDS)
    {};

MarketOrderBook::~MarketOrderBook() {
    // reset the internal data members
    mBids_by_price = nullptr;
    mAsks_by_price = nullptr;
    mOrder_id_to_oder.fill(nullptr);
}