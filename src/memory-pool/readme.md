# Memory Pool

Optimizes memory management for low-latency HFT performance by pre-allocating blocks.

## Purpose
- **Low Overhead:** Eliminates slow dynamic allocations (`malloc`/`free`) during runtime.
- **Deterministic Latency:** Ensures consistent, bounded execution times crucial for HFT.
- **Zero Fragmentation:** Prevents memory fragmentation over time by managing fixed-size blocks.
- **Thread Safety:** Generally supports concurrent access without synchronization bottlenecks using thread-local or lock-free designs.

## Key Components
- **Pool Size:** Total amount of pre-allocated memory.
- **Block Size:** Fixed size of each allocation unit.
- **Free List:** Tracks available memory blocks for quick reuse without searching.
- **Allocation Strategy:** Optimized methods for allocating and deallocating blocks.
- **Hardware Optimization:** Properly aligns memory to prevent false sharing and improve cache efficiency.

## Usage Example

```cpp
#include "memory-pool/memory_pool.h"

struct MyStruct {
    int id;
    double value;
    MyStruct() : id(0), value(0.0) {}
    MyStruct(int i, double v) : id(i), value(v) {}
};

int main() {
    // Pool of 100 elements
    MemoryPool<MyStruct> pool(100);
    
    // Allocate 
    MyStruct* obj = pool.Allocate(1, 3.14);
    
    // Deallocate
    pool.Deallocate(obj);
    
    return 0;
}
```