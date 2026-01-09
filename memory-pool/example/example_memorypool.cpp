/// \file example_memorypool.cpp
/// \brief Example demonstrating the usage of the MemoryPool template class
/// \details Shows how to allocate and deallocate primitive and custom struct
/// types
///          using the MemoryPool container with automatic memory management.

#include "memory-pool/memorypool.h"

/// \struct MyStruct
/// \brief A simple structure containing an array of integers
struct MyStruct {
    int d_[3];  ///< Array of 3 integers
};

/// \brief Main function demonstrating MemoryPool usage
/// \details Creates two memory pools (one for doubles, one for MyStruct),
///          allocates 50 elements to each, and deallocates every 5th element.
/// \return Exit status code (0 for success)
int main(int, char **) {
    /// Create a memory pool for 50 double values
    MemoryPool<double> prim_pool(50);
    /// Create a memory pool for 50 MyStruct objects
    MemoryPool<MyStruct> struct_pool(50);

    /// Allocate elements and demonstrate pool management
    for (auto i = 0; i < 50; ++i) {
        /// Allocate a double with value i to the primitive pool
        auto p_ret = prim_pool.allocate(i);
        /// Allocate a MyStruct with values {i, i+1, i+2} to the struct pool
        auto s_ret = struct_pool.allocate(MyStruct{i, i + 1, i + 2});

        /// Output the allocated primitive element and its address
        std::cout << "prim elem:" << *p_ret << " allocated at:" << p_ret
                  << std::endl;

        /// Output the allocated struct element values and its address
        std::cout << "struct elem:" << s_ret->d_[0] << "," << s_ret->d_[1]
                  << "," << s_ret->d_[2] << " allocated at:" << s_ret
                  << std::endl;

        /// Deallocate every 5th element from both pools
        if (i % 5 == 0) {
            std::cout << "deallocating prim elem:" << *p_ret
                      << " from:" << p_ret << std::endl;

            std::cout << "deallocating struct elem:" << s_ret->d_[0] << ", "
                      << s_ret->d_[1] << "," << s_ret->d_[2]
                      << " from:" << s_ret << std::endl;

            prim_pool.deallocate(p_ret);
            struct_pool.deallocate(s_ret);
        }
    }

    return 0;
}