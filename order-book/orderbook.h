#pragma once

// The important data members in this class are the following:
// Two mem pools, order_pool for MarketOrder objects and orders_at_price_pool
// for MarketOrdersAtPrice objects, are to be used to allocate and deallocate
// these objects as needed. The first pool, order_pool, is used to allocate and 
// deallocate MarketOrder objects. The second pool, orders_at_price_pool, is 
// used to allocate and deallocate MarketOrdersAtPrice objects. Remember that a 
// single MemPool instance is tied to a specific object type provided to it as a
// template parameter.

// A bbo variable of the BBO type, which will be used to compute and maintain a 
// BBO-view of the order book when there are updates and provided to any 
// components that require it.

// An oid_to_order variable of the OrderHashMap type will be used to track 
// MarketOrder objects by OrderId.

// A price_orders_at_price_ variable of the OrdersAtPriceHashMap type to track
// OrdersAtPrice objects by Price.

#include "utilities/types.h"
#include "utilities/macros.h"
#include "memory-pool/memorypool.h"
#include "market-orders/marketorder.h"

class MarketOrderBook final {
    public: 
      MarketOrderBook(TickerId ticker_id);

      auto onMarketUpdate()

    private:
    const TickerId mTicker_id;

    OrderHashMap mOrder_id_to_oder;
    MemoryPool<MarketOrderAtPrice> mOrders_at_price_pool;
    MarketOrderAtPrice *mBids_by_price = nullptr;
    MarketOrderAtPrice *mAsks_by_price = nullptr;

    OrdersAtPriceHashMap mPrice_orders_at_price;
    MemoryPool<MarketOrder> mOrder_pool;

    BestBidOffer mBestBidOffer;
    std::string mtime_str;
};

  /// Hash map from TickerId -> MarketOrderBook.
  typedef std::array<MarketOrderBook *, ME_MAX_TICKERS> MarketOrderBookHashMap;