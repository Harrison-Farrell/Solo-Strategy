#include <chrono>
#include <thread>

#include "lock-free-queue/lockfreequeue.h"

/// \struct MyStruct
/// \brief A simple structure containing an array of integers
struct MyStruct {
    int data[3];  ///< Array of 3 integers
};

/// \brief Consumer function that reads elements from a lock-free queue
/// \details Waits 5 seconds before starting to consume elements from the queue.
///          Reads elements one at a time with 1 second intervals until the
///          queue is empty.
/// \param[in] data_queue Pointer to the LockFreeQueue containing MyStruct
/// elements
auto consumeFunction(LockFreeQueue<MyStruct>* data_queue) {
    using namespace std::literals::chrono_literals;
    /// Wait 5 seconds before starting consumption
    std::this_thread::sleep_for(5s);

    /// Read and process elements while queue is not empty
    while (data_queue->size()) {
        /// Get the next readable element from the queue
        const auto d = data_queue->getNextRead();
        /// Advance the read index to mark element as consumed
        data_queue->updateReadIndex();

        /// Output the consumed element and current queue size
        std::cout << "consumeFunction read elem:" << d->data[0] << ","
                  << d->data[1] << "," << d->data[2]
                  << " size:" << data_queue->size() << std::endl;

        /// Wait 1 second before reading next element
        std::this_thread::sleep_for(1s);
    }

    std::cout << "consumeFunction exiting." << std::endl;
}

/// \brief Main function demonstrating lock-free queue usage with multiple
/// threads
/// \details Creates a lock-free queue with capacity of 20 elements and a
/// consumer thread.
///          The main thread produces 50 elements, and the consumer thread
///          consumes them concurrently. The queue wraps around when it reaches
///          capacity.
/// \return Exit status code (0 for success)
int main(int, char**) {
    /// Create a lock-free queue with capacity of 20 elements
    /// \note Queue is smaller than total elements produced, so it will wrap
    /// around
    LockFreeQueue<MyStruct> data_queue(20);

    /// Create consumer thread to read from the queue
    std::thread ct(consumeFunction, &data_queue);

    /// Produce 50 elements into the queue
    for (auto i = 0; i < 50; ++i) {
        /// Create a MyStruct element with values i, i*10, i*100
        const MyStruct d{i, i * 10, i * 100};
        /// Get write position and write the element
        *data_queue.getNextWrite() = d;
        /// Advance the write index to mark element as written
        data_queue.updateWriteIndex();

        /// Output the produced element and current queue size
        std::cout << "main constructed elem:" << d.data[0] << "," << d.data[1]
                  << "," << d.data[2] << " size:" << data_queue.size()
                  << std::endl;

        /// Wait 1 second before producing next element
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(1s);
    }

    /// Wait for consumer thread to complete
    ct.join();

    std::cout << "main exiting." << std::endl;

    return 0;
}
