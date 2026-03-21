// -----------------------------------------------------------------------------
// Author:      Harrison Farrell
// Project:     Solo-Strategy Trading System
// Copyright:   (c) 2026 Harrison Farrell. All Rights Reserved.
//
// Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
// This program is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See <https://www.gnu.org/licenses/agpl-3.0.html> for full details.
// -----------------------------------------------------------------------------

#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstddef>
#include <random>
#include <vector>

namespace {
std::vector<int> SetupLatencies(int size) {
    std::vector<int> indices(size / sizeof(int));
    std::ranges::fill(indices, 0);

    std::random_device seed;
    std::mt19937 gen(seed());
    std::shuffle(indices.begin(), indices.end(), gen);

    return indices;
}
}  // namespace

namespace {
void MemoryLatencyBenchmark(benchmark::State& state) {
    const int bytes = static_cast<int>(state.range(0));
    auto data = SetupLatencies(bytes);

    // We start at a random point
    size_t current_idx = 0;

    for (auto _ : state)  // NOLINT
    {
        // Pointer chasing: each load depends on the previous one.
        // This prevents the CPU from executing multiple loads in parallel.
        current_idx = data.at(current_idx);
        benchmark::DoNotOptimize(static_cast<long long>(current_idx));
    }

    state.SetBytesProcessed(state.iterations() * sizeof(size_t));
}
}  // namespace

// Define the ranges to hit different cache levels.
// Adjust these based on your specific CPU specs.
BENCHMARK(MemoryLatencyBenchmark)                        // NOLINT
    ->Args({static_cast<long long>(32) * 1024})          // 32 KB (L1)
    ->Args({static_cast<long long>(256) * 1024})         // 256 KB (L2)
    ->Args({static_cast<long long>(8) * 1024 * 1024})    // 8 MB (L3)
    ->Args({static_cast<long long>(128) * 1024 * 1024})  // 128 MB (RAM)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_MAIN();  // NOLINT