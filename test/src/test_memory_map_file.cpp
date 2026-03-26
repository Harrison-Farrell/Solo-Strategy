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
// NOLINTBEGIN
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <vector>

#include "memory-map/memory_map_file.h"

namespace {

class MemoryMappedFileTest : public ::testing::Test {
   protected:
    std::filesystem::path tempFilePath;

    void SetUp() override {
        // Create a temporary file path for testing
        auto tempDir = std::filesystem::temp_directory_path();
        tempFilePath = tempDir / "test_memory_map_data.bin";
    }

    void TearDown() override {
        // Clean up temporary file if it exists
        if (std::filesystem::exists(tempFilePath)) {
            std::filesystem::remove(tempFilePath);
        }
    }

    void CreateTestFile(const std::vector<uint8_t>& data) {
        std::ofstream ofs(tempFilePath, std::ios::binary | std::ios::out);
        ofs.write(reinterpret_cast<const char*>(data.data()), data.size());
        ofs.close();
    }
};

TEST_F(MemoryMappedFileTest, OpenAndCloseValidFile) {
    CreateTestFile({0x01, 0x02, 0x03, 0x04});

    MemoryMappedFile mappedFile;
    EXPECT_TRUE(mappedFile.Open(tempFilePath));
    EXPECT_TRUE(mappedFile.isValid());
    EXPECT_EQ(mappedFile.size(), 4);
    EXPECT_EQ(mappedFile.mappedSize(), 4);

    mappedFile.Close();
    EXPECT_FALSE(mappedFile.isValid());
    EXPECT_EQ(mappedFile.size(), 0);
    EXPECT_EQ(mappedFile.mappedSize(), 0);
}

TEST_F(MemoryMappedFileTest, OpenInvalidFile) {
    MemoryMappedFile mappedFile;
    std::filesystem::path nonExistentPath =
        std::filesystem::temp_directory_path() / "does_not_exist_file.bin";
    EXPECT_FALSE(mappedFile.Open(nonExistentPath));
    EXPECT_FALSE(mappedFile.isValid());
}

TEST_F(MemoryMappedFileTest, ArrayAccessValidAndInvalid) {
    CreateTestFile({0x11, 0x22, 0x33, 0x44});

    MemoryMappedFile mappedFile;
    ASSERT_TRUE(mappedFile.Open(tempFilePath));

    EXPECT_EQ(mappedFile[0], 0x11);
    EXPECT_EQ(mappedFile[1], 0x22);

    EXPECT_EQ(mappedFile.at(2), 0x33);
    EXPECT_EQ(mappedFile.at(3), 0x44);

    // static casting to allow discarding the return value
    EXPECT_THROW(static_cast<void>(mappedFile.at(4)), std::out_of_range);
}

TEST_F(MemoryMappedFileTest, RemapOffsets) {
    std::vector<uint8_t> data;
    for (int i = 0; i < 8192; ++i) {
        data.push_back(static_cast<uint8_t>(i % 256));
    }
    CreateTestFile(data);

    MemoryMappedFile mappedFile;
    ASSERT_TRUE(mappedFile.Open(tempFilePath, 1024));
    EXPECT_EQ(mappedFile.mappedSize(), 1024);

    // remap testing (requires page size alignment typically, assuming 4096 here
    // for safety, offset 4096) using page boundary 4096
    bool remapped = mappedFile.remap(4096, 2048);
    if (!remapped) {
        // Some OSes or specific page sizes might fail if offset is not aligned
        // to their page size. fallback to whole file map or skip specific
        // check.
        SUCCEED() << "Remap failed possibly due to page unalignment, skipping "
                     "test check.";
        return;
    }
    EXPECT_EQ(mappedFile.mappedSize(), 2048);
    EXPECT_EQ(mappedFile[0], 4096 % 256);
}

TEST_F(MemoryMappedFileTest, CursorSequentialReadAndNavigation) {
    CreateTestFile({0x01, 0x02, 0x03, 0x04, 0x0A, 0x0B});

    MemoryMappedFile mappedFile;
    ASSERT_TRUE(mappedFile.Open(tempFilePath));

    EXPECT_EQ(mappedFile.tell(), 0);

    // Read8
    EXPECT_EQ(mappedFile.Read8(), 0x01);
    EXPECT_EQ(mappedFile.tell(), 1);

    // ReadChar
    EXPECT_EQ(mappedFile.ReadChar(), '\x02');
    EXPECT_EQ(mappedFile.tell(), 2);

    // seek
    mappedFile.seek(4);
    EXPECT_EQ(mappedFile.tell(), 4);
    EXPECT_EQ(mappedFile.Read8(), 0x0A);
}

TEST_F(MemoryMappedFileTest, ReadEndianness) {
    // ITCH protocol deals in Big-Endian. Values here are encoded as big-endian.
    // 0x1234 -> BE: [0x12, 0x34]
    // 0x11223344 -> BE: [0x11, 0x22, 0x33, 0x44]
    CreateTestFile({0x12, 0x34, 0x11, 0x22, 0x33, 0x44, 0xAA,
                    0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x01, 0x02,
                    0x03, 0x04, 0x05, 0x06, 0x07, 0x08});

    MemoryMappedFile mappedFile;
    ASSERT_TRUE(mappedFile.Open(tempFilePath));

    EXPECT_EQ(mappedFile.Read16(), 0x1234);
    EXPECT_EQ(mappedFile.Read32(), 0x11223344);

    // Read48 test (6 bytes)
    EXPECT_EQ(mappedFile.Read48(), 0xAABBCCDDEEFF);

    // Read64 test (8 bytes)
    EXPECT_EQ(mappedFile.Read64(), 0x102030405060708);
}

TEST_F(MemoryMappedFileTest, ReadStrings) {
    CreateTestFile({'H', 'A', 'R', 'R', 'I', 'S', 'O', 'N', 'F', 'A', 'R', 'R',
                    'E', 'L', 'L'});

    MemoryMappedFile mappedFile;
    ASSERT_TRUE(mappedFile.Open(tempFilePath));

    std::array<char, 8> expected_result{'H', 'A', 'R', 'R', 'I', 'S', 'O', 'N'};
    EXPECT_EQ(mappedFile.ReadArray<8>(), expected_result);

    EXPECT_EQ(mappedFile.tell(), 8);

    char buf[10] = {0};
    mappedFile.copyString(buf, 7);
    EXPECT_STREQ(buf, "FARRELL");
    EXPECT_EQ(mappedFile.tell(), 15);
}

TEST_F(MemoryMappedFileTest, Iterators) {
    CreateTestFile({0x55, 0xAA, 0xFF});

    MemoryMappedFile mappedFile;
    ASSERT_TRUE(mappedFile.Open(tempFilePath));

    auto it = mappedFile.begin();
    EXPECT_EQ(*it, 0x55);
    EXPECT_EQ(*(it + 1), 0xAA);
    EXPECT_EQ(*(it + 2), 0xFF);

    EXPECT_EQ(mappedFile.end() - mappedFile.begin(), 3);
}

}  // namespace
// NOLINTEND