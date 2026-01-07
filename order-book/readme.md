# Order Book

An order book is a real-time, electronic list of buy (bid) and sell (ask) orders for a financial instrument (e.g., stocks, futures, or cryptocurrencies), organized by price level. Its primary purpose is to provide transparency into market liquidity and depth, allowing traders to:

* View the best available prices for buying (highest bid) and selling (lowest ask).
* Assess supply and demand at different price points.
* Execute trades efficiently, especially in high-frequency trading (HFT) where low-latency access to order book data is critical for strategies like arbitrage or market making.
* In HFT ecosystems, order books are often accessed via APIs or direct exchange feeds to minimize latency and inform algorithmic decisions. For more details, refer to your active file's checklist on order types and exchanges.

An order book is a real-time, electronic list of buy (bid) and sell (ask) orders for a financial instrument (e.g., stocks, futures, or cryptocurrencies), organized by price level. Its primary purpose is to provide transparency into market liquidity and depth, allowing traders to:

* View the best available prices for buying (highest bid) and selling (lowest ask).
* Assess supply and demand at different price points.
* Execute trades efficiently, especially in high-frequency trading (HFT) where low-latency access to order book data is critical for strategies like arbitrage or market making.
* In HFT ecosystems, order books are often accessed via APIs or direct exchange feeds to minimize latency and inform algorithmic decisions. For more details, refer to your active file's checklist on order types and exchanges.

## Key Components

An order book in trading systems, especially for HFT, consists of the following key components:

* **Bids (Buy Orders):** A list of buy orders sorted by price in descending order (highest price first). Each entry includes:
  * Price level
  * Total quantity (volume) at that price
  * Optionally, individual order details (e.g., order ID, timestamp, trader ID)

* **Asks (Sell Orders):** A list of sell orders sorted by price in ascending order (lowest price first). Each entry mirrors the bids structure.

* **Price Levels:** Discrete price points where orders are aggregated, showing cumulative volume for liquidity assessment.

* **Order Queue:** At each price level, a queue of individual orders (FIFO or priority-based) to handle matching and execution.

* **Market Depth:** The number of visible price levels (e.g., top 10 bids/asks), which varies by exchange.

In C++ implementations for low-latency HFT, order books are often represented using data structures like `std::map` or custom priority queues for efficient insertion, deletion, and lookups. For example, bids might use a red-black tree keyed by price. Refer to your active orderbook.md file for more context on usage.