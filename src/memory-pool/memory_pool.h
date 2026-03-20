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

/**
 * @file memory_pool.h
 * @brief High-performance object pool for avoiding dynamic memory allocation.
 */

#ifndef SOLO_STRATEGY_SRC_MEMORY_POOL_MEMORY_POOL_H_
#define SOLO_STRATEGY_SRC_MEMORY_POOL_MEMORY_POOL_H_

#include <cstdint>
#include <string>
#include <vector>

#include "utilities/macros.h"

/**
 * @brief Pre-allocates objects in memory to avoid the overhead of dynamic allocation.
 * Offers significantly better performance than standard heap allocation.
 * @tparam T The type of objects managed by the pool.
 */
template <typename T>
class MemoryPool final {
   public:
    /**
     * @brief Constructs a MemoryPool with a fixed capacity.
     * @param num_elems The number of elements to pre-allocate.
     */
    explicit MemoryPool(std::size_t num_elems)
        : mStore(num_elems, {T(), true}) /* pre-allocation of vector storage. */ {
        ASSERT(reinterpret_cast<const ElementBlock*>(&(mStore[0].element)) == &(mStore[0]),
               "T object should be first member of ElementBlock.");
    }

    /**
     * @brief Allocates an object in-place from the pool.
     * Finds the next free block and constructs the object using placement new.
     * @tparam Args Constructor argument types.
     * @param args Arguments to forward to the object's constructor.
     * @return Pointer to the allocated and constructed object.
     */
    template <typename... Args>
    T* allocate(Args... args) noexcept {
        auto obj_block = &(mStore[mNext_free_index]);
        ASSERT(obj_block->is_free,
               "Expected free ObjectBlock at index:" + std::to_string(mNext_free_index));
        T* ret = &(obj_block->element);
        ret = new (ret) T(args...);  // placement new.
        obj_block->is_free = false;

        updateNextFreeIndex();

        return ret;
    }

    /**
     * @brief Deallocates an object, marking its block as free for reuse.
     * @param elem Pointer to the object to deallocate.
     */
    auto deallocate(const T* elem) noexcept {
        const auto elem_index = (reinterpret_cast<const ElementBlock*>(elem) - &mStore[0]);
        ASSERT(elem_index >= 0 && static_cast<size_t>(elem_index) < mStore.size(),
               "Element being deallocated does not belong to this Memory pool.");
        ASSERT(!mStore[elem_index].is_free,
               "Expected in-use ObjectBlock at index:" + std::to_string(elem_index));
        mStore[elem_index].is_free = true;
    }

    // Deleted default, copy & move constructors and assignment-operators.
    MemoryPool() = delete;
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool(const MemoryPool&&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&&) = delete;

   private:
    /**
     * @brief Updates the next free index for subsequent allocations.
     */
    auto updateNextFreeIndex() noexcept {
        const auto initial_free_index = mNext_free_index;
        while (!mStore[mNext_free_index].is_free) {
            ++mNext_free_index;
            if (mNext_free_index == mStore.size()) [[unlikely]] {
                mNext_free_index = 0;
            }
            if (initial_free_index == mNext_free_index) [[unlikely]] {
                ASSERT(initial_free_index != mNext_free_index, "Memory Pool out of space.");
            }
        }
    }

    /**
     * @brief Internal structure for each element block in the pool.
     */
    struct ElementBlock {
        T element;
        bool is_free = true;
    };

    /** @brief Index tracking the next free element. */
    size_t mNext_free_index = 0;

    /** @brief Underlying storage for the pool elements. */
    std::vector<ElementBlock> mStore;
};

#endif  // SOLO_STRATEGY_SRC_MEMORY_POOL_MEMORY_POOL_H_