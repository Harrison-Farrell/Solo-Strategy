# Thread Affinity

Provides utilities for low-latency thread management, specifically enabling thread pinning to isolate critical path threads on specific CPU cores.

## Purpose
- **CPU Isolation:** CPU pinning ensures that a high-frequency trading thread is not preempted or migrated across cores by the OS scheduler, avoiding cache invalidation.
- **Latency Consistency:** By dedicating a core to a specific workload (like the matching engine or network polling), execution jitter is drastically reduced.

## Key Components
- **`SetThreadCore()`**: An OS-agnostic helper (supporting both Windows `SetThreadAffinityMask` and Linux `pthread_setaffinity_np`) to lock the current thread to a hardware core ID.
- **`CreateAndStartThread()`**: A wrapper that spins up a new `std::thread`, applies the core affinity mask immediately upon startup, and executes the provided function.

## Usage Example

```cpp
#include "thread/thread.h"
#include <iostream>

void HighFrequencyWorker(int id) {
    std::cout << "Worker " << id << " running on pinned core." << std::endl;
    // ... latency-sensitive loop ...
}

int main() {
    int core_id = 2; // Pin to CPU Core 2
    
    // Create and start the thread, pin it, and pass arguments
    auto worker_thread = CreateAndStartThread(core_id, "HFT_Worker_Thread", HighFrequencyWorker, 42);
    
    worker_thread->join();
    return 0;
}
```
