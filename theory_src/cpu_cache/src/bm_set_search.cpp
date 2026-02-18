#include <algorithm>
#include <random>
#include <set>

#include <benchmark/benchmark.h>

// simple set search function
bool setSearch(const std::set<int> &data_set, int target) {
  auto search = data_set.find(target);
  return search != data_set.end();
}

// benchmark for set search
static void createSet(benchmark::State &state) {
  // construct an intial set of the correct size
  auto set = std::set<int>();
  // iterate through the set settings the values incrementally
  for (int i = 0; i < state.range(); ++i) {
    set.insert(i);
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distr(0, state.range());

  // randomly set the target
  int target = distr(gen);

  for (auto _ : state) {
    // code inside this loop is benchmarked
    bool result = setSearch(set, target);
    benchmark::DoNotOptimize(result);
  }
}

// register the benchmark
BENCHMARK(createSet)->RangeMultiplier(2)->Range(2, 64);

// standard marco to run all registered benchmarks
BENCHMARK_MAIN();
