# Lock-Free Queue

A high-performance concurrent queue designed for ultra-low latency inter-thread communication in high-frequency trading (HFT) environments.

## Purpose
- **Ultra-low Latency**: Avoids OS-level locks to eliminate context switching and lock contention overhead.
- **Predictable Performance**: Minimizes jitter by ensuring continuous, system-wide progress.
- **Deadlock-Free**: Inherently safe from deadlocks without the use of traditional synchronization primitives.

## Key Mechanisms
- **Atomic Operations**: Utilizes Compare-and-Swap (CAS) for non-blocking state updates.
- **Memory Ordering**: Employs memory fences to ensure sequential visibility of memory accesses across threads.
- **Ring Buffer**: Uses a fixed-size circular array with atomic indices for producers and consumers.
- **Hardware Optimization**: Aligns on CPU cache lines and pads structures to prevent false sharing.

## Usage Example

```cpp
#include "lock-free-queue/lock_free_queue.h"
#include <iostream>

int main() {
    LockFreeQueue<int> queue(1024);
    
    // Producer
    queue.push(42);
    
    // Consumer
    int value;
    if (queue.pop(value)) {
        std::cout << "Popped: " << value << std::endl;
    }
    
    return 0;
}
```