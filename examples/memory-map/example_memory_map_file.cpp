/*
 * --------------------------------------------------------------------------
 * Author:      Harrison Farrell
 * Project:     Solo-Strategy Trading System
 * Copyright:   (c) 2026 Harrison Farrell. All Rights Reserved.
 *
 * Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
 * This program is distributed WITHOUT ANY WARRANTY; without even the 
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See <https://www.gnu.org/licenses/agpl-3.0.html> for full details.
 * --------------------------------------------------------------------------
 */

#include <bitset>
#include <iostream>
#include <sstream>
#include <string>

#include "itch-parser/memory-map/memory_map_file.h"

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        std::cout << "Invalid Arguments!" << std::endl;
        std::cout << "supply the binary file path to read " << std::endl;
        exit(1);
    }

    std::string input_file_path = argv[1];
    std::cout << "File Path to  read: " << argv[1] << std::endl;

    MemoryMappedFile mapped_file(input_file_path);

    std::cout << "Offset:Value \t" << std::bitset<8>{mapped_file.read8()} << std::endl;
    std::cout << "Offset:Value \t" << std::bitset<16>{mapped_file.read16()} << std::endl;
    std::cout << "Offset:Value \t" << std::bitset<32>{mapped_file.read32()} << std::endl;
    std::cout << "Offset:Value \t" << std::bitset<64>{mapped_file.read64()} << std::endl;
    return 0;
}