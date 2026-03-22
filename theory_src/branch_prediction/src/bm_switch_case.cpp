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

namespace {
void Sink(int& val) { benchmark::DoNotOptimize(val += 1); }

void SwitchStatementBenchmark(benchmark::State& state) {
    const int case_to_find = static_cast<int>(state.range(0));
    int dummy_value = 0;

    for (auto _ : state)  // NOLINT
    {
        switch (case_to_find) {
            case 0:
                Sink(dummy_value);
                break;
            case 1:
                Sink(dummy_value);
                break;
            case 2:
                Sink(dummy_value);
                break;
            case 3:
                Sink(dummy_value);
                break;
            case 4:
                Sink(dummy_value);
                break;
            case 5:
                Sink(dummy_value);
                break;
            case 6:
                Sink(dummy_value);
                break;
            case 7:
                Sink(dummy_value);
                break;
            case 8:
                Sink(dummy_value);
                break;
            case 9:
                Sink(dummy_value);
                break;
            case 10:
                Sink(dummy_value);
                break;
            case 11:
                Sink(dummy_value);
                break;
            case 12:
                Sink(dummy_value);
                break;
            case 13:
                Sink(dummy_value);
                break;
            case 14:
                Sink(dummy_value);
                break;
            case 15:
                Sink(dummy_value);
                break;
            case 16:
                Sink(dummy_value);
                break;
            case 17:
                Sink(dummy_value);
                break;
            case 18:
                Sink(dummy_value);
                break;
            default:
                benchmark::DoNotOptimize(dummy_value--);
                break;
        }
    }

    state.SetBytesProcessed(state.iterations() * sizeof(int));
}
}  // namespace

// Register the benchmark and test multiple cases
// This helps see if 'case 9' takes longer than 'case 0'
BENCHMARK(SwitchStatementBenchmark)->Arg(0)->Arg(5)->Arg(9)->Arg(18);  // NOLINT

BENCHMARK_MAIN();  // NOLINT