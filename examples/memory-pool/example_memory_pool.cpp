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

// system includes
#include <array>
#include <iostream>
// local includes
#include "memory-pool/memory_pool.h"

namespace {
struct MyStruct {
    std::array<int, 3> data;
};
}  // namespace

int main() {
    constexpr int pool_size = 50;
    constexpr int deallcate_modulus = 5;
    constexpr int iteration_loop_size = 50;

    MemoryPool<double> prim_pool(pool_size);
    MemoryPool<MyStruct> struct_pool(pool_size);

    for (auto i = 0; i < iteration_loop_size; ++i) {
        auto* p_ret = prim_pool.Allocate(i);

        auto* s_ret = struct_pool.Allocate(MyStruct{i, i + 1, i + 2});

        std::cout << "prim elem:" << *p_ret << " Allocated at:" << p_ret
                  << "\n";

        std::cout << "struct elem:" << s_ret->data.at(0) << ","
                  << s_ret->data.at(1) << "," << s_ret->data.at(2)
                  << " Allocated at:" << s_ret << "\n";

        if (i % deallcate_modulus == 0) {
            std::cout << "deallocating prim elem:" << *p_ret
                      << " from:" << p_ret << "\n";

            std::cout << "deallocating struct elem:" << s_ret->data.at(0)
                      << ", " << s_ret->data.at(1) << "," << s_ret->data.at(2)
                      << " from:" << s_ret << "\n";

            prim_pool.Deallocate(p_ret);
            struct_pool.Deallocate(s_ret);
        }
    }

    return 0;
}