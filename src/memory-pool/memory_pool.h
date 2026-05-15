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

/// @file memory_pool.h
/// @brief High-performance object pool for avoiding dynamic memory allocation.

#ifndef SOLO_STRATEGY_SRC_MEMORY_POOL_MEMORY_POOL_H_
#define SOLO_STRATEGY_SRC_MEMORY_POOL_MEMORY_POOL_H_

#include <string>
#include <vector>

#include "utilities/macros.h"

/// @brief Pre-Allocates objects in memory to avoid the overhead of dynamic
/// allocation. Offers significantly better performance than standard heap
/// allocation.
/// @tparam T The type of objects managed by the pool.
template <typename T>
class MemoryPool final {
   public:
    ~MemoryPool() = default;
    MemoryPool() = delete;
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool(const MemoryPool&&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&&) = delete;

    /// @brief Constructs a MemoryPool with a fixed capacity.
    /// @param num_elems The number of elements to pre-Allocate.
    explicit MemoryPool(std::size_t num_elems)
        : m_Store(num_elems,
                  {T(), true}) /* pre-allocation of vector storage. */ {
        ASSERT(reinterpret_cast<const ElementBlock*>(&(m_Store[0].element)) ==
                   &(m_Store[0]),
               "T object should be first member of ElementBlock.");
    }

    /// @brief Allocates an object in-place from the pool.
    /// Finds the next free block and constructs the object using placement new.
    /// @tparam Args Constructor argument types.
    /// @param args Arguments to forward to the object's constructor.
    /// @return Pointer to the Allocated and constructed object.
    template <typename... Args>
    T* Allocate(Args... args) noexcept {
        auto obj_block = &(m_Store[m_NextFreeIndex]);
        ASSERT(obj_block->is_free, "Expected free ObjectBlock at index:" +
                                       std::to_string(m_NextFreeIndex));
        T* ret = &(obj_block->element);
        ret = new (ret) T(args...);  // placement new.
        obj_block->is_free = false;

        UpdateNextFreeIndex();

        return ret;
    }

    /// @brief DeAllocates an object, marking its block as free for reuse.
    /// @param elem Pointer to the object to Deallocate.
    auto Deallocate(const T* elem) noexcept {
        const auto elem_index =
            (reinterpret_cast<const ElementBlock*>(elem) - &m_Store[0]);
        ASSERT(
            elem_index >= 0 && static_cast<size_t>(elem_index) < m_Store.size(),
            "Element being DeAllocated does not belong to this Memory pool.");
        ASSERT(!m_Store[elem_index].is_free,
               "Expected in-use ObjectBlock at index:" +
                   std::to_string(elem_index));
        m_Store[elem_index].is_free = true;
    }

   private:
    /// @brief Updates the next free index for subsequent allocations.
    auto UpdateNextFreeIndex() noexcept {
        const auto initial_free_index = m_NextFreeIndex;
        while (!m_Store[m_NextFreeIndex].is_free) {
            ++m_NextFreeIndex;
            if (m_NextFreeIndex == m_Store.size()) [[unlikely]] {
                m_NextFreeIndex = 0;
            }
            if (initial_free_index == m_NextFreeIndex) [[unlikely]] {
                ASSERT(initial_free_index != m_NextFreeIndex,
                       "Memory Pool out of space.");
            }
        }
    }

    /// @brief Internal structure for each element block in the pool.
    struct ElementBlock {
        T element;
        bool is_free = true;
    };

    /// @brief Index tracking the next free element.
    size_t m_NextFreeIndex = 0;

    /// @brief Underlying storage for the pool elements.
    std::vector<ElementBlock> m_Store;
};

#endif  // SOLO_STRATEGY_SRC_MEMORY_POOL_MEMORY_POOL_H_