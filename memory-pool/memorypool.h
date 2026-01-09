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
    explicit MemoryPool(std::size_t element_size);

    /// \brief Allocates an object in-place from the pool.
    ///
    /// Finds the next free block, constructs the object in-place, and returns a
    /// pointer.
    /// \tparam Args Constructor argument types.
    /// \param args Arguments to forward to the object's constructor.
    /// \return Pointer to the allocated object.
    template <typename... Args>
    T *allocate(Args... args) noexcept;

    /// \brief Deallocates an object, marking its block as free.
    /// \param element Pointer to the object to deallocate.
    auto deallocate(const T *element) noexcept;

    // Deleted default, copy & move constructors and assignment-operators.
    MemoryPool() = delete;
    MemoryPool(const MemoryPool &) = delete;
    MemoryPool(const MemoryPool &&) = delete;
    MemoryPool &operator=(const MemoryPool &) = delete;
    MemoryPool &operator=(const MemoryPool &&) = delete;

   private:
    /// \brief Updates the next free index to be written over with new
    /// information.
    auto updateNextFreeIndex() noexcept;

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