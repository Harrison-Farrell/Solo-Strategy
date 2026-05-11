# Order Book

A real-time, electronic ledger of buy (bid) and sell (ask) orders organized by price level. It provides transparency into market liquidity and depth for efficient execution in high-frequency trading (HFT).

## Key Components
- **Bids (Buy Orders):** Sorted descending by price (highest price first).
- **Asks (Sell Orders):** Sorted ascending by price (lowest price first).
- **Price Levels:** Discrete points aggregating cumulative order volume at that specific price.
- **Order Queue:** Priority or FIFO queues handling matching execution at each price level.
- **Market Depth:** The visible price levels (e.g., top 10).

Designed for ultra-low latency access, typically using fast associative structures (e.g., `std::map`, red-black trees) or array-based priority queues for fast insertion, deletion, and cross-matching lookups.

## Usage Example

```cpp
#include "order-book/order_book.h"
#include "market-orders/market_update.h"

int main() {
    TickerId ticker = 1;
    OrderBook order_book(ticker);
    
    MarketUpdate update;
    // ... populate update with order details ...
    order_book.OnMarketUpdate(&update);
    
    // Update Best Bid and Offer
    order_book.UpdateBestBidOffer(true, true);
    const BestBidOffer* bbo = order_book.GetBestBidOffer();
    
    return 0;
}
```