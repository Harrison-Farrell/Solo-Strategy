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
        createAndStartThread(0, "Worker_0", heavyWork, 0, 10'000));
    threads.push_back(
        createAndStartThread(1, "Worker_1", heavyWork, 1, 20'000));

    // Wait for threads to finish their execution
    for (auto* t : threads) {
        if (t && t->joinable()) {
            t->join();
            delete t;  // Clean up the raw pointer allocated by
                       // createAndStartThread
        }
    }

    std::cout << "--- Application Finished Successfully ---" << std::endl;
    return 0;
}