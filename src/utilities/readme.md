# Utilities

Contains common cross-cutting definitions, constants, and helper functions shared across multiple modules within the trading system.

## Purpose
- **Centralized Typings:** Providing global `using` aliases (e.g., `OrderId`, `TickerId`, `Price`, `Qty`) to guarantee consistent type sizes and memory footprints across the core engine.
- **System Limits:** Declaring `constexpr` maximum depth bounds and capacities (like lock-free queue sizes) for compile-time allocations.
- **Assertion and Error Handling:** Implementing critical macros such as `ASSERT` and `FATAL` used across the architecture to quickly exit upon constraint violation.
- **Cross-Platform Endianness:** Providing highly optimized, native-endian agnostic conversions (via `byteswap`) for Network Byte Order (Big-Endian) data parsing.

## Key Components
- **`types.h`**: Defines the `constexpr` invalid-markers (e.g., `Price_INVALID`) alongside string representations and side enums (`Side::BUY` / `Side::SELL`).
- **`macros.h`**: Provides `ASSERT()` and `FATAL()` which immediately flush stderr and perform a `std::quick_exit` to bypass large unwinding times on catastrophic failures.
- **`endian_utils.h`**: Templates for `Swap`, `FromBigEndian`, and `FromLittleEndian` powered by `std::byteswap` and `std::endian`.

## Usage Example

```cpp
#include "utilities/types.h"
#include "utilities/macros.h"
#include "utilities/endian_utils.h"

int main() {
    // Asserting constraints using fast system-exit macros
    ASSERT(ME_MAX_TICKERS <= 256, "Tickers exceeded bounds.");

    // Using typed limits
    Price price = Price_INVALID;
    
    // Converting Big-Endian data to Host layout 
    uint32_t network_data = 0x0A0B0C0D;
    uint32_t host_data = endian::FromBigEndian(network_data);

    return 0;
}
```
