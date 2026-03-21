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
// simple linear search function
bool LinearSearch(std::vector<int>& data_set, int target) {
    return std::ranges::contains(data_set, target);
}
}  // namespace

namespace {
std::vector<int> SetupLatencies(int size) {
    std::vector<int> indices(size);
    std::ranges::fill(indices, 0);

    std::random_device seed;
    std::mt19937 gen(seed());
    std::shuffle(indices.begin(), indices.end(), gen);

    return indices;
}
}  // namespace

namespace {
// benchmark for linear search
void Linear(benchmark::State& state) {
    // construct an intial vector of the correct size
    auto set = SetupLatencies(static_cast<int>(state.range()));

    std::random_device seed;
    std::mt19937 gen(seed());
    std::uniform_int_distribution<> distr(0, static_cast<int>(state.range()));

    // randomly set the target
    const int target = distr(gen);

    for (auto _ : state)  // NOLINT
    {
        // code inside this loop is benchmarked
        bool result = LinearSearch(set, target);
        benchmark::DoNotOptimize(result);
    }
}
}  // namespace

// register the benchmark
BENCHMARK(Linear)->RangeMultiplier(2)->Range(2, 64);  // NOLINT

// standard marco to run all registered benchmarks
BENCHMARK_MAIN();  // NOLINT
