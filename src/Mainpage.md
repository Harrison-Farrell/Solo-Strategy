# Solo-Strategy Trading System

## Project Summary

Solo-Strategy is a high-performance, low-latency trading system ecosystem designed for high-frequency trading (HFT) applications. This project serves a dual purpose: it is a practical platform for learning the intricacies of High-Frequency Trading systems and an exercise to improve and master C++ development skills. This includes with aligning to core C++ Software principles and IDIOMs such as 'SOLID', 'DRY', 'Coupling & Cohesion' principles and the RAII IDIOM.

- S: Single Responsibility Principle
- O: Open-closed Principle
- L: Liskov Substitution Principle
- I: Interface Segregation Principle
- D: Dependency Inversion Principle
----- ----- ----- ----- -----
The 'DRY' 
- D: Don't
- R: Repeat
- Y: Yourself
----- ----- ----- ----- -----
- Coupling and Cohesion

### Core Components

*   **[ITCH Parser](@ref MemoryMappedFile):** High-efficiency Nasdaq ITCH protocol parsing using memory-mapped files and safe binary data handling with automatic endian conversion.
*   **[Lock-Free Queue](@ref LockFreeQueue):** A thread-safe, lock-free circular buffer for low-latency communication between components, ensuring minimal synchronization overhead.
*   **[Memory Pool](@ref MemoryPool):** Deterministic memory management using pre-Allocated object pools to avoid the costs of dynamic heap allocation during runtime.
*   **[Order Book](@ref MarketOrderBook):** Advanced Limit Order Book (LOB) implementation using efficient linked lists and memory-mapped data structures for real-time order processing.

## Architecture

The system is designed with a modular approach, where each component is optimized for speed and reliability. By abstracting platform-specific details and focusing on cache-friendly data structures, Solo-Strategy provides a robust foundation for building sophisticated trading strategies.
