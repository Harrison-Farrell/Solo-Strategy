#pragma once

#include <iostream>
#include <vector>
#include <atomic>
#include <string>

#include "utilities/macros.h"

template<typename T>
class LockFreeQueue final {
    public:

        LockFreeQueue(std::size_t element_number)
        : mStore(element_number, T()) {} // pre-allocate vector size

        auto getNextWrite() noexcept {
            return &mStore[mNext_write];
        }

        auto updateWriteIndex() noexcept {
            mNext_write = (mNext_write + 1) % mStore.size();
            mSize++;
        }

        auto getNextRead() const noexcept -> const T* {
            return(size() ? &mStore[mNext_read] : nullptr);
        }

        auto updateReadIndex() noexcept {
            mNext_read = (mNext_read + 1) % mStore.size();
            ASSERT(mSize != 0,  "Read an invalid element");
            mSize--;
        }

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
        std::vector<T> mStore;
        std::atomic<size_t> mNext_write = {0};
        std::atomic<size_t> mNext_read  = {0};
        std::atomic<size_t> mSize = {0};
};