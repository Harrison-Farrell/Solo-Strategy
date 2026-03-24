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

#include <thread>
#include <vector>

#include "thread/thread.h"

// A dummy function representing some heavy business logic
void heavyWork(int id, int iterations) {
    long long counter = 0;
    for (int i = 0; i < iterations; ++i) {
        counter += i;
    }
    std::cout << "Thread [" << id << "] finished computing. Result: " << counter
              << std::endl;
}

int main() {
    std::cout << "--- Starting Cross-Platform Thread Affinity Example ---"
              << std::endl;

    // Create a pool of threads pinned to different cores (Core 0, Core 1, etc.)
    std::vector<std::thread*> threads;

    threads.push_back(
        CreateAndStartThread(0, "Worker_0", heavyWork, 0, 10'000));
    threads.push_back(
        CreateAndStartThread(1, "Worker_1", heavyWork, 1, 20'000));

    // Wait for threads to finish their execution
    for (auto* t : threads) {
        if (t && t->joinable()) {
            t->join();
            delete t;  // Clean up the raw pointer allocated by
                       // CreateAndStartThread
        }
    }

    std::cout << "--- Application Finished Successfully ---" << std::endl;
    return 0;
}