#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "utilities/macros.h"

/// \brief MemoryPool pre-allocates objects to memory to avoid dynamic
/// allocation.
///
/// Offers significant time advantages (10x to 100x faster than dynamic
/// allocation). For example, allocating 1,000 objects via new might take
/// ~50-500 μs, while pre-allocated access could be <5 μs.
/// \tparam T The type of objects managed by the pool.
template <typename T>
class MemoryPool final {
   public:
    /// \brief Constructs a MemoryPool with a fixed number of elements.
    /// \param element_size The number of elements to pre-allocate.
    explicit MemoryPool(std::size_t num_elems)
        : mStore(num_elems,
                 {T(), true}) /* pre-allocation of vector storage. */ {
        ASSERT(reinterpret_cast<const ElementBlock *>(&(mStore[0].element)) ==
                   &(mStore[0]),
               "T object should be first member of ElementBlock.");
    }

    /// \brief Allocates an object in-place from the pool.
    ///
    /// Finds the next free block, constructs the object in-place, and returns a
    /// pointer.
    /// \tparam Args Constructor argument types.
    /// \param args Arguments to forward to the object's constructor.
    /// \return Pointer to the allocated object.
    template <typename... Args>
    T *allocate(Args... args) noexcept {
        auto obj_block = &(mStore[mNext_free_index]);
        ASSERT(obj_block->is_free, "Expected free ObjectBlock at index:" +
                                       std::to_string(mNext_free_index));
        T *ret = &(obj_block->element);
        ret = new (ret) T(args...);  // placement new.
        obj_block->is_free = false;

        updateNextFreeIndex();

        return ret;
    }

    /// \brief Deallocates an object, marking its block as free.
    /// \param element Pointer to the object to deallocate.
    auto deallocate(const T *elem) noexcept {
        const auto elem_index =
            (reinterpret_cast<const ElementBlock *>(elem) - &mStore[0]);
        ASSERT(
            elem_index >= 0 && static_cast<size_t>(elem_index) < mStore.size(),
            "Element being deallocated does not belong to this Memory pool.");
        ASSERT(!mStore[elem_index].is_free,
               "Expected in-use ObjectBlock at index:" +
                   std::to_string(elem_index));
        mStore[elem_index].is_free = true;
    }

    // Deleted default, copy & move constructors and assignment-operators.
    MemoryPool() = delete;
    MemoryPool(const MemoryPool &) = delete;
    MemoryPool(const MemoryPool &&) = delete;
    MemoryPool &operator=(const MemoryPool &) = delete;
    MemoryPool &operator=(const MemoryPool &&) = delete;

   private:
    /// \brief Updates the next free index to be written over with new
    /// information.
    auto updateNextFreeIndex() noexcept {
        const auto initial_free_index = mNext_free_index;
        while (!mStore[mNext_free_index].is_free) {
            ++mNext_free_index;
            if (mNext_free_index == mStore.size())
                [[unlikely]] {  // hardware branch predictor
                                // should almost always predict
                                // this to be false any ways.
                mNext_free_index = 0;
            }
            if (initial_free_index == mNext_free_index) [[unlikely]] {
                ASSERT(initial_free_index != mNext_free_index,
                       "Memory Pool out of space.");
            }
        }
    }

    /// \brief Structure for each element within the memory pool.
    struct ElementBlock {
        /// \brief The actual object.
        T element;
        /// \brief Indicates if the block is free.
        bool is_free = true;
    };

    /// \brief Index to track the next free element.
    size_t mNext_free_index = 0;

    /// \brief Underlying storage for the pool elements.
    std::vector<ElementBlock> mStore;
};