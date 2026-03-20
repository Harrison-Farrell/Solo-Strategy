[![Linux OS Build](https://github.com/Harrison-Farrell/Solo-Strategy/actions/workflows/linux_build.yml/badge.svg)](https://github.com/Harrison-Farrell/Solo-Strategy/actions/workflows/linux_build.yml)
[![Windows OS Build](https://github.com/Harrison-Farrell/Solo-Strategy/actions/workflows/windows_build.yml/badge.svg)](https://github.com/Harrison-Farrell/Solo-Strategy/actions/workflows/windows_build.yml)
[![Unit Tests](https://github.com/Harrison-Farrell/Solo-Strategy/actions/workflows/unit_tests.yml/badge.svg)](https://github.com/Harrison-Farrell/Solo-Strategy/actions/workflows/unit_tests.yml)

# Trading Ecosystem Self-Learning Checklist

This checklist outlines the key topics to master when building a foundational understanding of the HFT ecosystem from a low-latency C++ perspective.

Source References
https://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHspecification.pdf
https://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/moldudp64.pdf
https://www.asxonline.com/content/dam/asxonline/public/documents/asx-trade-refresh-manuals/asx-trade-itch-message-specification.pdf


https://gitlab.inf.unibz.it/Sebastian.Cavada/Machine-Learning-for-Algorithmic-Trading-Second-Edition_Original/-/blob/v2/02_market_and_fundamental_data/01_NASDAQ_TotalView-ITCH_Order_Book/01_build_itch_order_book.py

https://simontoth.substack.com/p/daily-bite-of-c-optimizing-code-to?r=1g4l8a&utm_campaign=post&utm_medium=web&triedRedirect=true 

https://github.com/runk/itch/tree/master
https://github.com/bbalouki/itchcpp/tree/main


* [ ] **[Memory Pool](memory-pool/readme.md):** Pre-Allocated block of memory to reduce allocation overhead and ensure deterministic performance in low-latency HFT systems.
* [ ] **[Lock Free Queue](lock-free-queue/readme.md):** A concurrent data structure designed to facilitate communication between different threads
* [ ] **[Market Orders](market-orders):** The structures used to contain the information for each order. To be consumed by the Order books
* [ ] **[Order Book](order-book/readme.md):** Electronic list of buy (bid) and sell (ask) orders for a financial instrument organized by price level.