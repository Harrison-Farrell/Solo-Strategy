# Memory Mapped File

Provides a high-performance, read-only memory mapping of a file to memory page boundaries. It is designed to allow the application to access file content as if it were an in-memory array, which is significantly faster than traditional I/O.

## Purpose
- **High Performance ITCH Parsing:** Reading massive binary files (like ITCH tick data) efficiently by mapping them directly into the process's address space.
- **Cross-Platform:** Supports both Windows (`MapViewOfFile`) and Unix-like (`mmap`) systems seamlessly.
- **Endian Conversion:** Provides built-in methods to read sequential data with automatic conversions from Network Byte Order (Big-Endian) to the host system's native format.

## Key Components
- **OS-Level Mapping:** Directly leverages `mmap` or `CreateFileMapping` for fast accesses.
- **Cursor-Based Reads:** Contains continuous read functions (`Read8`, `Read16`, `Read32`, `Read64`, `ReadSymbol`) for parsing tightly packed binary structs.
- **Cache Hints:** Allows passing performance hints to the OS (e.g., sequential scanning) to optimize prefetching behavior.

## Usage Example

```cpp
#include "memory-map/memory_map_file.h"
#include <iostream>

int main() {
    MemoryMappedFile file("market_data.bin");
    
    if (file.isValid()) {
        // Read a 16-bit big-endian integer from the current cursor position
        uint16_t message_length = file.Read16();
        
        // Read the next 8 bytes as a character array (symbol)
        auto symbol = file.ReadSymbol();
        
        std::cout << "Parsed symbol of length: " << message_length << std::endl;
    }
    
    return 0;
}
```
