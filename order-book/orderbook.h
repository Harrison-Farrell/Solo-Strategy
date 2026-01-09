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

    /// \brief Processes a market update message.
    /// \param market_update Pointer to the market update.
    /// \return void
    auto onMarketUpdate(const MEMarketUpdate *market_update) noexcept -> void;

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
    BestBidOffer mBestBidOffer;
    /// \brief Time string for the order book.
    std::string mtime_str;

   private:
    /// \brief Maps a price to an index for O(1) lookup.
    /// \param price The price to map.
    /// \return Index value.
    inline auto priceToIndex(Price price) const noexcept {
        return price % ME_MAX_PRICE_LEVELS;
    }

    /// \brief Fetches the MarketOrdersAtPrice corresponding to a price.
    /// \param price The price to look up.
    /// \return Pointer to MarketOrderAtPrice.
    auto getOrdersAtPrice(Price price) const noexcept -> MarketOrderAtPrice * {
        return mPrice_orders_at_price.at(priceToIndex(price));
    }

    // TODO Page 303 / 304
    // https://github.com/PacktPublishing/Building-Low-Latency-Applications-with-CPP/blob/main/Chapter12/trading/strategy/market_order_book.h
};

/// \typedef MarketOrderBookHashMap
/// \brief Hash map from TickerId to MarketOrderBook.
typedef std::array<MarketOrderBook *, ME_MAX_TICKERS> MarketOrderBookHashMap;