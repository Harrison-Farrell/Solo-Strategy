#pragma once

#include <atomic>
#include <iostream>
#include <string>
#include <vector>

#include "utilities/macros.h"

/// \brief A lock-free, thread-safe queue for sharing information between
/// threads.
///
/// This class implements a fixed-size circular buffer using atomic operations
/// to allow multiple threads to safely enqueue and dequeue elements without
/// locks.
/// \tparam T The type of elements stored in the queue.
template <typename T>
class LockFreeQueue final {
   public:
    /// \brief Constructs a LockFreeQueue with a fixed number of elements.
    /// \param element_number The maximum number of elements the queue can hold.
    LockFreeQueue(std::size_t element_number)
        : mStore(element_number, T()) {}  // pre-allocate vector size

    /// \brief Gets a pointer to the next writable element in the queue.
    /// \return Pointer to the next writable element.
    auto getNextWrite() noexcept { return &mStore[mNext_write]; }

    /// \brief Updates the write index after writing an element.
    ///
    /// Advances the write index in a circular fashion and increments the size.
    auto updateWriteIndex() noexcept {
        mNext_write = (mNext_write + 1) % mStore.size();
        mSize++;
    }

    /// \brief Gets a pointer to the next readable element in the queue.
    /// \return Pointer to next readable element, or nullptr if queue is empty.
    auto getNextRead() const noexcept -> const T * {
        return (size() ? &mStore[mNext_read] : nullptr);
    }

    /// \brief Updates the read index after reading an element.
    ///
    /// Advances the read index in a circular fashion and decrements the size.
    auto updateReadIndex() noexcept {
        mNext_read = (mNext_read + 1) % mStore.size();
        ASSERT(mSize != 0, "Read an invalid element");
        mSize--;
    }

    /// \brief Returns the current number of elements in the queue.
    /// \return The number of elements currently stored in the queue.
    auto size() const noexcept {
        // atomically loads & returns the current value for atomic variable
        return mSize.load();
    }

    // Deleted default, copy & move constructors and assignment-operators.
    LockFreeQueue() = delete;
    LockFreeQueue(const LockFreeQueue &) = delete;
    LockFreeQueue(const LockFreeQueue &&) = delete;
    LockFreeQueue &operator=(const LockFreeQueue &) = delete;
    LockFreeQueue &operator=(const LockFreeQueue &&) = delete;

   private:
    /// \brief The underlying storage for the queue elements.
    std::vector<T> mStore;
    /// \brief The index for the next write operation.
    std::atomic<size_t> mNext_write = {0};
    /// \brief The index for the next read operation.
    std::atomic<size_t> mNext_read = {0};
    /// \brief The current number of elements in the queue.
    std::atomic<size_t> mSize = {0};
};