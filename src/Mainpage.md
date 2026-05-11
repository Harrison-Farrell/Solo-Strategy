# Solo-Strategy Trading System

## Overview
Solo-Strategy is a high-performance, low-latency ecosystem for high-frequency trading (HFT). It serves as both a practical HFT learning platform and a rigorous exercise in modern, optimized C++ development.

## Design Intent
The core design philosophy focuses on deterministic, ultra-low latency execution through mechanical sympathy and strict adherence to modern C++ principles:
- **Zero-Cost Abstractions**: Leveraging templates and compile-time evaluation to minimize runtime overhead.
- **Deterministic Memory**: Avoiding dynamic allocation (`new`/`delete`) on the critical path via strictly pre-allocated memory pools.
- **Lock-Free Concurrency**: Utilizing atomic operations instead of OS-level locks to eliminate context switching and execution jitter.
- **Clean Architecture**: Maintaining high cohesion and low coupling through strict adherence to SOLID principles, DRY, and RAII standard idioms. 

## Core Components
* **[ITCH Parser](@ref ITCH_Parser):** High-efficiency Nasdaq ITCH protocol parser with safe binary data handling and automatic endian conversion.
* **[Lock-Free Queue](@ref LockFreeQueue):** Thread-safe, lock-free circular buffer for minimal-overhead inter-thread communication.
* **[Memory Pool](@ref MemoryPool):** Pre-allocated object pools delivering predictable, rapid memory management.
* **[Order Book](@ref OrderBook):** Advanced Limit Order Book (LOB) implementation utilizing cache-friendly structures for real-time order processing.
