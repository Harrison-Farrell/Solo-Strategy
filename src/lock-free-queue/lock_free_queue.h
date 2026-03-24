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

/// \file lock_free_queue.h
/// \brief Thread-safe, lock-free circular buffer for low-latency data passing.

#ifndef SOLO_STRATEGY_SRC_LOCK_FREE_QUEUE_LOCK_FREE_QUEUE_H_
#define SOLO_STRATEGY_SRC_LOCK_FREE_QUEUE_LOCK_FREE_QUEUE_H_

#include <atomic>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "utilities/macros.h"

/// \brief A lock-free, thread-safe queue for sharing data between threads.
/// Uses a fixed-Size circular buffer with atomic operations for lock-free
/// enqueuing and dequeuing.
/// \tparam T The type of elements stored in the queue.
template <typename T>
class LockFreeQueue final {
   public:
    /// \brief Constructs a LockFreeQueue with a fixed capacity.
    /// \param element_number The maximum number of elements the queue can hold.
    LockFreeQueue(std::size_t element_number) : m_Store(element_number, T()) {}

    /// \brief Gets a pointer to the next writable element in the queue.
    /// \return Pointer to the next writable element.
    auto GetNextWrite() noexcept { return &m_Store[m_NextWrite]; }

    /// \brief Advances the write index in a circular fashion and increments the
    /// size. Should be called after writing to the pointer returned by
    /// GetNextWrite().
    auto UpdateWriteIndex() noexcept {
        m_NextWrite = (m_NextWrite + 1) % m_Store.size();
        m_Size++;
    }

    /// \brief Gets a pointer to the next readable element in the queue.
    /// \return Pointer to next readable element, or nullptr if queue is empty.
    auto GetNextRead() const noexcept -> const T* {
        return (Size() ? &m_Store[m_NextRead] : nullptr);
    }

    /// \brief Advances the read index in a circular fashion and decrements the
    /// size. Should be called after reading from the pointer returned by
    /// GetNextRead().
    auto UpdateReadIndex() noexcept {
        m_NextRead = (m_NextRead + 1) % m_Store.size();
        ASSERT(m_Size != 0, "Read an invalid element");
        m_Size--;
    }

    /// \brief Pushes an element onto the queue using move semantics.
    /// \param value The element to push.
    /// \return true if successful, false if the queue is full.
    bool Push(T&& value) noexcept {
        if (Size() >= m_Store.size()) {
            return false;
        }
        // bound check is managed through modulus function
        m_Store[m_NextWrite] = std::move(value);  // NOLINT
        UpdateWriteIndex();
        return true;
    }

    /// \brief Pushes an element onto the queue using copy semantics.
    /// \param value The element to push.
    /// \return true if successful, false if the queue is full.
    bool Push(const T& value) noexcept {
        if (Size() >= m_Store.size()) {
            return false;
        }

        m_Store[m_NextWrite] = value;
        UpdateWriteIndex();
        return true;
    }

    /// \brief Pops an element from the queue.
    /// \param value Reference to store the popped element.
    /// \return true if successful, false if the queue is empty.
    bool Pop(T& value) noexcept {
        if (Size() == 0) {
            return false;
        }

        value = std::move(m_Store[m_NextRead]);
        UpdateReadIndex();
        return true;
    }

    /// \brief Returns the current number of elements in the queue.
    /// \return The number of elements currently stored in the queue.
    auto Size() const noexcept { return m_Size.load(); }

    LockFreeQueue() = delete;
    LockFreeQueue(const LockFreeQueue&) = delete;
    LockFreeQueue(const LockFreeQueue&&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&&) = delete;

   private:
    /// \brief The underlying storage for the queue elements.
    std::vector<T> m_Store;
    /// \brief The index for the next write operation.
    std::atomic<size_t> m_NextWrite = {0};
    /// \brief The index for the next read operation.
    std::atomic<size_t> m_NextRead = {0};
    /// \brief The current number of elements in the queue.
    std::atomic<size_t> m_Size = {0};
};

#endif  // SOLO_STRATEGY_SRC_LOCK_FREE_QUEUE_LOCK_FREE_QUEUE_H_