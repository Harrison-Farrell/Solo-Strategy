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

#include <bitset>
#include <cstdlib>
#include <iostream>
#include <print>
#include <string>

#include "memory-map/memory_map_file.h"

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::println("Invalid Arguments!");
        std::println("Supply the binary file path to read");
        std::quick_exit(1);
    }

    // disable cppcoreguidelines-pro-bounds-pointer-arithmetic
    const std::string input_file_path(argv[1]);  // NOLINT

    std::println("File Path to  read: {}", input_file_path);

    MemoryMappedFile mapped_file(input_file_path);

    constexpr int bit_08 = 8;
    constexpr int bit_16 = 16;
    constexpr int bit_32 = 32;
    constexpr int bit_64 = 64;

    std::cout << "Offset:Value \t" << std::bitset<bit_08>{mapped_file.read8()} << "\n";
    std::cout << "Offset:Value \t" << std::bitset<bit_16>{mapped_file.read16()} << "\n";
    std::cout << "Offset:Value \t" << std::bitset<bit_32>{mapped_file.read32()} << "\n";
    std::cout << "Offset:Value \t" << std::bitset<bit_64>{mapped_file.read64()} << "\n";

    return 0;
}