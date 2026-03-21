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
#include <random>
#include <vector>

namespace {
// a simple binary search function
bool BinarySearch(const std::vector<int>& data_set, int target) {
    return std::ranges::binary_search(data_set, target);
}
}  // namespace

namespace {
// Benchmark for Binary Search
void Binary(benchmark::State& state) {
    auto set = std::vector<int>(state.range());

    for (int i = 0; i < set.size(); ++i) {
        set.at(i) = i;
    }
    std::random_device seed;
    std::mt19937 gen(seed());
    std::uniform_int_distribution<> distr(0, static_cast<int>(state.range()));
    auto target = distr(gen);

    for (auto _ : state)  // NOLINT
    {
        bool result = BinarySearch(set, target);
        benchmark::DoNotOptimize(result);
    }
}
}  // namespace
// Register the benchmark
BENCHMARK(Binary)->RangeMultiplier(2)->Range(2, 64);  // NOLINT

// Standard macro to run all registered benchmarks
BENCHMARK_MAIN();  // NOLINT