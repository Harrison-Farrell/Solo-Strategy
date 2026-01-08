#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "utilities/macros.h"

// the purpose of the memory pool is to pre-allocate objects to memory to avoid
// the needs to perform dynamic allocation. It offers significant time
// advantages. Roughly 10x to 100x faster.
// For example, allocating 1,000 objects via new might take ~50-500 μs,
// while pre-allocated access could be <5 μs.
template <typename T>
class MemoryPool final {
   public:
    // constructs a vector with count copies of elements with value value
    explicit MemoryPool(std::size_t element_size)
        : mStore(element_size, {T(), true}) {
        ASSERT(reinterpret_cast<const ElementBlock *>(&(mStore[0].element)) ==
                   &(mStore[0]),
               "T object should be first member.");
    }

    // finds the next free block. Contrusting the object in-place
    // returns a pointer to the new allocated object
    template <typename... Args>
    T *allocate(Args... args) noexcept {
        auto block = &(mStore[mNext_free_index]);

        ASSERT(block->is_free, "Expected free Element at index:" +
                                   std::to_string(mNext_free_index));

        T *element_ptr = &(block->element);
        element_ptr = new (element_ptr) T(args...);
        block->is_free = false;

        updateNextFreeIndex();

        return element_ptr;
    }

    // using the element block pointer. Simply resets the is_free flag.
    auto deallocate(const T *element) noexcept {
        const auto elem_index =
            (reinterpret_cast<const ElementBlock *>(element) - &mStore[0]);

        ASSERT(
            elem_index >= 0 && static_cast<size_t>(elem_index) < mStore.size(),
            "Element does not belong to this pool.");

        ASSERT(
            !mStore[elem_index].is_free,
            "Expected in-use Element at index:" + std::to_string(elem_index));

        mStore[elem_index].is_free = true;
    }

    // Deleted default, copy & move constructors and assignment-operators.
    MemoryPool() = delete;
    MemoryPool(const MemoryPool &) = delete;
    MemoryPool(const MemoryPool &&) = delete;
    MemoryPool &operator=(const MemoryPool &) = delete;
    MemoryPool &operator=(const MemoryPool &&) = delete;

   private:
    // find the next free index to be written over with new information
    auto updateNextFreeIndex() noexcept {
        const auto free_index = mNext_free_index;
        while (!mStore[mNext_free_index].is_free) {
            ++mNext_free_index;  // pre-increment instead of post-increment
            if (mNext_free_index == mStore.size()) [[unlikely]] {
                mNext_free_index = 0;
            }
            if (free_index == mNext_free_index) [[unlikely]] {
                ASSERT(free_index != mNext_free_index,
                       "Memory Pool out of space.");
            }
        }
    }

    // the structure to be used for each element within the memory pool
    struct ElementBlock {
        T element;
        bool is_free = true;
    };

    // the index to track the next free index
    size_t mNext_free_index = 0;

    // could've chosen to use a std::array that would allocate the memory
    // on the stack instead of the heap. Would have to measure to see which one
    // yields better performance. It is good to have objects on the stack but
    // performance starts getting worse as the size of the pool increases.
    std::vector<ElementBlock> mStore;
};