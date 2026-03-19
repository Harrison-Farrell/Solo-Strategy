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

/**
 * @file lock_free_queue.h
 * @brief Thread-safe, lock-free circular buffer for low-latency data passing.
 */

#ifndef SOLO_STRATEGY_SRC_LOCK_FREE_QUEUE_LOCK_FREE_QUEUE_H_
#define SOLO_STRATEGY_SRC_LOCK_FREE_QUEUE_LOCK_FREE_QUEUE_H_

#include <atomic>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "utilities/macros.h"

/**
 * @brief A lock-free, thread-safe queue for sharing data between threads.
 * Uses a fixed-size circular buffer with atomic operations for lock-free
 * enqueuing and dequeuing.
 * @tparam T The type of elements stored in the queue.
 */
template <typename T>
class LockFreeQueue final {
   public:
    /**
     * @brief Constructs a LockFreeQueue with a fixed capacity.
     * @param element_number The maximum number of elements the queue can hold.
     */
    LockFreeQueue(std::size_t element_number) : mStore(element_number, T()) {}

    /**
     * @brief Gets a pointer to the next writable element in the queue.
     * @return Pointer to the next writable element.
     */
    auto getNextWrite() noexcept { return &mStore[mNext_write]; }

    /**
     * @brief Advances the write index in a circular fashion and increments the size.
     * Should be called after writing to the pointer returned by getNextWrite().
     */
    auto updateWriteIndex() noexcept {
        mNext_write = (mNext_write + 1) % mStore.size();
        mSize++;
    }

    /**
     * @brief Gets a pointer to the next readable element in the queue.
     * @return Pointer to next readable element, or nullptr if queue is empty.
     */
    auto getNextRead() const noexcept -> const T* {
        return (size() ? &mStore[mNext_read] : nullptr);
    }

    /**
     * @brief Advances the read index in a circular fashion and decrements the size.
     * Should be called after reading from the pointer returned by getNextRead().
     */
    auto updateReadIndex() noexcept {
        mNext_read = (mNext_read + 1) % mStore.size();
        ASSERT(mSize != 0, "Read an invalid element");
        mSize--;
    }

    /**
     * @brief Pushes an element onto the queue using move semantics.
     * @param value The element to push.
     * @return true if successful, false if the queue is full.
     */
    bool push(T&& value) noexcept {
        if (size() >= mStore.size()) {
            return false;
        }

        mStore[mNext_write] = std::move(value);
        updateWriteIndex();
        return true;
    }

    /**
     * @brief Pushes an element onto the queue using copy semantics.
     * @param value The element to push.
     * @return true if successful, false if the queue is full.
     */
    bool push(const T& value) noexcept {
        if (size() >= mStore.size()) {
            return false;
        }

        mStore[mNext_write] = value;
        updateWriteIndex();
        return true;
    }

    /**
     * @brief Pops an element from the queue.
     * @param value Reference to store the popped element.
     * @return true if successful, false if the queue is empty.
     */
    bool pop(T& value) noexcept {
        if (size() == 0) {
            return false;
        }

        value = std::move(mStore[mNext_read]);
        updateReadIndex();
        return true;
    }

    /**
     * @brief Returns the current number of elements in the queue.
     * @return The number of elements currently stored in the queue.
     */
    auto size() const noexcept { return mSize.load(); }

    LockFreeQueue() = delete;
    LockFreeQueue(const LockFreeQueue&) = delete;
    LockFreeQueue(const LockFreeQueue&&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&&) = delete;

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

#endif  // SOLO_STRATEGY_SRC_LOCK_FREE_QUEUE_LOCK_FREE_QUEUE_H_