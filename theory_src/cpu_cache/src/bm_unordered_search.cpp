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
#include <iostream>
#include <random>
#include <unordered_set>


// simple unordered_set search function
bool unorderedSearch(const std::unordered_set<int>& data_set, int target) {
    auto search = data_set.find(target);
    return search != data_set.end();
}

// benchmark for unordered_set search
static void createUnorderedSet(benchmark::State& state) {
    // construct an initial set of the correct size
    auto set = std::unordered_set<int>();
    // iterate through the set settings the values incrementally
    for (int i = 0; i < state.range(); ++i) {
        set.insert(i);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, state.range());

    // randomly set the target
    int target = distr(gen);

    std::cout << "Bucket Count: " << set.bucket_count() << std::endl;
    for (auto _ : state) {
        // code inside this loop is benchmarked
        bool result = unorderedSearch(set, target);
        benchmark::DoNotOptimize(result);
    }
}

// register the benchmark
BENCHMARK(createUnorderedSet)->RangeMultiplier(2)->Range(8, 1024);

// standard marco to run all registered benchmarks
BENCHMARK_MAIN();
