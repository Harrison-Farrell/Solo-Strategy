# Market Orders

Defines the core structures for individual orders and matching engine updates used throughout the trading system. 

## Purpose
- **Order Definition:** Provides the foundational `MarketOrder` structure that populates the Limit Order Book.
- **Price Level Aggregation:** Provides the `MarketOrderAtPrice` structure which acts as a doubly-linked list node for a specific price level, aggregating individual orders at that price.
- **Update Messages:** Defines `MEMarketUpdate` and `MDPMarketUpdate` structures used to communicate order additions, modifications, executions, and cancellations between system components.

## Key Components
- **`MarketOrder`**: A single limit order containing ID, side, price, quantity, priority, and intrusive linked-list pointers.
- **`MarketOrderAtPrice`**: Groups all `MarketOrder`s at a specific price limit to facilitate fast matching and queue management.
- **`BestBidOffer`**: A summarized view of the Top-of-Book (BBO) containing the best bid/ask prices and aggregated quantities.
- **Market Updates**: Packed structs ensuring consistent binary layout for publishing events lock-free through queues.

## Usage Example

```cpp
#include "market-orders/market_order.h"
#include "market-orders/market_update.h"
#include <iostream>

int main() {
    // Create a new BUY market order at price 100 with quantity 50
    MarketOrder order(12345, Side::BUY, 100, 50, 1, nullptr, nullptr);
    
    std::cout << order.toString() << std::endl;
    
    // Create an update message for the matching engine
    MEMarketUpdate update;
    update.type = MarketUpdateType::ADD;
    update.order_id = 12345;
    update.side = Side::BUY;
    update.price = 100;
    
    std::cout << update.toString() << std::endl;

    return 0;
}
```
