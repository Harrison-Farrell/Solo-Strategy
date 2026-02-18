#include <benchmark/benchmark.h>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>

// This function creates a "shuffled" linked list within a vector.
// Each element contains the index of the next element to visit.
std::vector<size_t> setup_latencies(size_t size) {
    std::vector<size_t> indices(size / sizeof(size_t));
    std::iota(indices.begin(), indices.end(), 0);
    
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);
    
    return indices;
}

static void BM_MemoryLatency(benchmark::State& state) {
    const size_t bytes = state.range(0);
    auto data = setup_latencies(bytes);
    
    // We start at a random point
    size_t current_idx = 0;
    
    for (auto _ : state) {
        // Pointer chasing: each load depends on the previous one.
        // This prevents the CPU from executing multiple loads in parallel.
        current_idx = data[current_idx];
        benchmark::DoNotOptimize(current_idx);
    }
    
    state.SetBytesProcessed(state.iterations() * sizeof(size_t));
}

// Define the ranges to hit different cache levels.
// Adjust these based on your specific CPU specs.
BENCHMARK(BM_MemoryLatency)
    ->Args({32 * 1024})       // 32 KB (L1)
    ->Args({256 * 1024})      // 256 KB (L2)
    ->Args({8 * 1024 * 1024}) // 8 MB (L3)
    ->Args({128 * 1024 * 1024}) // 128 MB (RAM)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_MAIN();