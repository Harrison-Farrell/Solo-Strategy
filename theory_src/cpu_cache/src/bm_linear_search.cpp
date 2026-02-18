#include <algorithm>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

// simple linear search function
bool linearSearch(std::vector<int> &data_set, int target) {
  if (std::find(data_set.begin(), data_set.end(), target) != data_set.end()) {
    return true;
  }
  return false;
}

std::vector<int> setup_latencies(int size) {
  std::vector<int> indices(size);
  std::iota(indices.begin(), indices.end(), 0);

  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(indices.begin(), indices.end(), g);

  return indices;
}

// benchmark for linear search
static void linear(benchmark::State &state) {
  // construct an intial vector of the correct size
  auto set = setup_latencies(state.range());

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distr(0, state.range());

  // randomly set the target
  int target = distr(gen);

  for (auto _ : state) {
    // code inside this loop is benchmarked
    bool result = linearSearch(set, target);
    benchmark::DoNotOptimize(result);
  }
}

// register the benchmark
BENCHMARK(linear)->RangeMultiplier(2)->Range(2, 64);

// standard marco to run all registered benchmarks
BENCHMARK_MAIN();
