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

// system includes
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
// local includes
#include "thread/thread.h"

namespace {
// A dummy function representing some heavy business logic
void HeavyWork(int id, int iterations) {
    long long counter = 0;
    for (int i = 0; i < iterations; ++i) {
        counter += i;
    }
    std::cout << "Thread [" << id << "] finished computing. Result: " << counter
              << "\n";
}
}  // namespace

int main() {
    std::cout << "Starting Cross-Platform Thread Affinity Example" << "\n";

    constexpr int worker_one_iterations = 10000;
    constexpr int worker_two_iterations = 30000;

    // Create a pool of threads pinned to different cores (Core 0, Core 1, etc.)
    std::vector<std::shared_ptr<std::thread>> threads;

    threads.emplace_back(CreateAndStartThread(0, "Worker_0", HeavyWork, 0,
                                              worker_one_iterations));

    threads.emplace_back(CreateAndStartThread(1, "Worker_1", HeavyWork, 1,
                                              worker_two_iterations));

    for (const auto& worker : threads) {
        worker->join();
    }

    std::cout << "Application Finished Successfully" << "\n ";
    return 0;
}