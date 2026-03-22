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

#include <functional>
#include <unordered_map>

namespace {
// Dummy work function
void Sink(int& val) { benchmark::DoNotOptimize(val += 1); }

// Global or static map to simulate a persistent lookup table
const std::unordered_map<int, std::function<void(int&)>> CaseMap = {
    {0, Sink},  {1, Sink},  {2, Sink},  {3, Sink},  {4, Sink},  {5, Sink},  {6, Sink},
    {7, Sink},  {8, Sink},  {9, Sink},  {10, Sink}, {11, Sink}, {12, Sink}, {13, Sink},
    {14, Sink}, {15, Sink}, {16, Sink}, {17, Sink}, {18, Sink}

};

void HashMapLookupBenchmark(benchmark::State& state) {
    const int case_to_find = static_cast<int>(state.range(0));
    int dummy_value = 0;

    for (auto _ : state)  // NOLINT
    {
        // Find the "case" in the map
        auto iterator = CaseMap.find(case_to_find);

        if (iterator != CaseMap.end()) {
            // Execute the mapped function
            iterator->second(dummy_value);
        } else {
            benchmark::DoNotOptimize(dummy_value--);
        }
    }

    state.SetBytesProcessed(state.iterations() * sizeof(int));
}
}  // namespace

// Register the benchmark for the same cases as the switch
BENCHMARK(HashMapLookupBenchmark)->Arg(0)->Arg(5)->Arg(9)->Arg(18);  // NOLINT

BENCHMARK_MAIN();  // NOLINT