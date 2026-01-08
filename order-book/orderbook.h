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

#include "market-orders/marketorder.h"
#include "market-orders/marketupdate.h"
#include "memory-pool/memorypool.h"
#include "utilities/macros.h"
#include "utilities/types.h"

class MarketOrderBook final {
   public:
    MarketOrderBook(TickerId ticker_id);

    ~MarketOrderBook();

    auto onMarketUpdate(const MEMarketUpdate *market_update) noexcept -> void;

   private:
    // the ticker id to for the instructment
    const TickerId mTicker_id;

    // an array of orders The indexed by their order id
    OrderHashMap mOrder_id_to_oder;
    // memory pool to allocate orders at a particular price
    MemoryPool<MarketOrderAtPrice> mOrders_at_price_pool;
    // Head of the bids
    MarketOrderAtPrice *mBids_by_price = nullptr;
    // head of the asks
    MarketOrderAtPrice *mAsks_by_price = nullptr;
    // an array of orders at a price indexed by their price
    OrdersAtPriceHashMap mPrice_orders_at_price;
    // memory pool to allocate singular market orders
    MemoryPool<MarketOrder> mOrder_pool;

    BestBidOffer mBestBidOffer;
    std::string mtime_str;

   private:
    // Price-to-index Mapping "O(1)" lookup. Using Module as a "Circular Map"
    inline auto priceToIndex(Price price) const noexcept {
        return price % ME_MAX_PRICE_LEVELS
    }
    // Fetch and return the MarketOrdersAtPrice corresponding to a price
    auto getOrdersAtPrice(Price price) const noexcept -> MarketOrderAtPrice * {
        return mPrice_orders_at_price.at(priceToIndex(price));
    }
};

/// Hash map from TickerId -> MarketOrderBook.
typedef std::array<MarketOrderBook *, ME_MAX_TICKERS> MarketOrderBookHashMap;