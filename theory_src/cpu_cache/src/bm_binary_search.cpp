/*
 * ------------------------------------------------------------------------------
 * Author:      Harrison Farrell
 * Project:     Solo-Strategy Trading System
 * Copyright:   (c) 2026 Harrison Farrell. All Rights Reserved.
 *
 * Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
 * This program is distributed WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See <https://www.gnu.org/licenses/agpl-3.0.html> for full details.
 * ------------------------------------------------------------------------------
 */

#include <benchmark/benchmark.h>

#include <algorithm>
#include <random>
#include <vector>

// a simple binary search function
bool binary_search(const std::vector<int>& data_set, int target) {
    return std::binary_search(data_set.begin(), data_set.end(), target);
}

// Benchmark for Binary Search
static void Binary(benchmark::State& state) {
    auto set = std::vector<int>(state.range());

    for (int i = 0; i < set.size(); ++i) {
        set[i] = i;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, state.range());
    auto target = distr(gen);

    for (auto _ : state) {
        bool result = binary_search(set, target);
        benchmark::DoNotOptimize(result);
    }
}
// Register the benchmark
BENCHMARK(Binary)->RangeMultiplier(2)->Range(2, 64);

// Standard macro to run all registered benchmarks
BENCHMARK_MAIN();
