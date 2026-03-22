# Benchmark Analysis: Large Switch vs. Hash Map Lookup

This document outlines the architectural and performance trade-offs between using a C++ `switch` statement and a `std::unordered_map` for dispatching logic based on a key.

---

## 1. The Large Switch Case
The `switch` statement is a language-level construct that allows the compiler to perform deep static analysis and hardware-level optimizations.

### Pros
* **Zero-Cost Abstraction:** For dense cases, the compiler generates a **Jump Table**, which is a simple array of memory addresses. Finding a case is a $O(1)$ operation consisting of a single addition and an indirect jump.
* **Inlining:** The compiler can easily inline the code within each `case`, avoiding the overhead of a function call.
* **Branch Prediction:** Modern CPUs are highly optimized for hardware-level branching and will "learn" the jump pattern if it is repetitive.
* **No Heap Allocation:** The entire structure exists in the "text" (code) segment of the binary.

### Cons
* **Binary Size:** A massive switch statement (thousands of cases) can lead to "instruction cache pressure," slowing down the CPU as it struggles to keep the code in the L1 cache.
* **Static Only:** You cannot add or remove cases at runtime; the logic is baked into the executable.
* **Maintainability:** Extremely long files can become difficult for humans to navigate and read.

---

## 2. The Hash Map Lookup (`std::unordered_map`)
A hash map moves the dispatch logic into a data structure, mapping keys to function pointers or `std::function` objects.

### Pros
* **Dynamic Flexibility:** You can register, unregister, or swap "cases" (functions) at runtime without recompiling.
* **Cleaner Code:** Decouples the dispatch logic from the business logic. Each "case" can live in its own file, class, or even a dynamically loaded library (DLL/SO).
* **Sparse Keys:** If your IDs are non-contiguous (e.g., 1, 1005, 999999), a map is more memory-efficient than a massive, sparse jump table.

### Cons
* **Hashing Overhead:** Every lookup requires calculating a hash of the key and traversing a bucket list, which is significantly slower than a simple pointer jump.
* **Indirect Calls:** Using `std::function` or function pointers usually prevents the compiler from inlining the logic, forcing a "context switch" for the CPU.
* **Memory Overhead:** Requires heap allocation for the nodes and the bucket array, increasing the risk of memory fragmentation.
* **Cache Misses:** Map nodes are often scattered in memory, leading to expensive data cache misses compared to the contiguous memory of a jump table.

---

## Comparison Summary

| Feature | Large Switch | Hash Map |
| :--- | :--- | :--- |
| **Lookup Speed** | **Fastest** ($O(1)$ or $O(\log n)$) | **Slower** ($O(1)$ average) |
| **Memory Location** | Code Segment (Instruction Cache) | Heap (Data Cache) |
| **Runtime Changes** | Impossible