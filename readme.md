| Linux OS Build | Windows OS Build | Unit Tests |
| --- | --- | --- |
| [![Linux OS Build](https://github.com/Harrison-Farrell/Solo-Strategy/actions/workflows/linux_build.yml/badge.svg)](https://github.com/Harrison-Farrell/Solo-Strategy/actions/workflows/linux_build.yml) | [![Windows OS Build](https://github.com/Harrison-Farrell/Solo-Strategy/actions/workflows/windows_build.yml/badge.svg)](https://github.com/Harrison-Farrell/Solo-Strategy/actions/workflows/windows_build.yml) | [![Unit Tests](https://github.com/Harrison-Farrell/Solo-Strategy/actions/workflows/unit_tests.yml/badge.svg)](https://github.com/Harrison-Farrell/Solo-Strategy/actions/workflows/unit_tests.yml) |

# Solo-Strategy Trading System

Solo-Strategy is a high-performance, low-latency algorithmic trading ecosystem built in C++. It focuses on deterministic execution, ultra-fast market data parsing, order book reconstruction, and efficient lock-free concurrency for High-Frequency Trading (HFT).

## System Architecture Pipeline

```text
+--------------------+        +--------------------+        +--------------------+
|    Read Worker     |        |    ITCH Parser     |        |   Books Manager    |
|     (Core 0)       |        |     (Core 1)       |        |     (Core 2)       |
+--------------------+        +--------------------+        +--------------------+
| Reads binary feed  |        | Reads raw packets  |        | Reads messages     |
| via MemoryMap file |        | Parses into ITCH   |        | Updates & builds   |
| extracts payload   |        | Message structs    |        | order books        |
+---------+----------+        +---------+----------+        +---------+----------+
          |                             |                             |
          |       +-------------+       |       +-------------+       |
          +-----> | input_queue | ------+-----> |  msg_queue  | ------+
                  | LockFree    |               | LockFree    |
                  | Queue       |               | Queue       |
                  +-------------+               +-------------+
                  <RawItchPacket>               <ITCH::Message>
```

## Modular Ecosystem Checklist

This repository is constructed from several core modules. Review the Readme for each component to master the foundational concepts of this low-latency C++ trading architecture:

* [ ] **[ITCH Parser](src/itch-parser/readme.md):** Parses NASDAQ ITCH 5.0 protocol messages to extract market data directly from binary feeds.
* [ ] **[Lock-Free Queue](src/lock-free-queue/readme.md):** A high-performance concurrent queue designed for inter-thread communication.
* [ ] **[Market Orders](src/market-orders/readme.md):** Defines order structures, price level aggregations, and matching engine update events.
* [ ] **[Memory Mapped File](src/memory-map/readme.md):** Cross-platform memory-mapped file to read blocks of binary data without I/O overhead.
* [ ] **[Memory Pool](src/memory-pool/readme.md):** Pre-allocates memory blocks to prevent dynamic allocation fragmentation improving latency.
* [ ] **[Order Book](src/order-book/readme.md):** Aggregates limit buy (bid) and sell (ask) updates ordered by price and FIFO priority.
* [ ] **[Thread Affinity](src/thread/readme.md):** OS-agnostic CPU thread isolation (pinning), ensuring critical-path logic runs on dedicated cores.
* [ ] **[Utilities](src/utilities/readme.md):** System-wide type aliases, catastrophic failure assert macros, and endian-network swap helpers.
