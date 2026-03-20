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

#include "utilities/endian_utils.h"

#include <gtest/gtest.h>

#include <bit>
#include <cstdint>

namespace {

using namespace endian;

TEST(EndianUtilsTest, SwapUInt8) {
    EXPECT_EQ(Swap<uint8_t>(0xA5), 0xA5);
    EXPECT_EQ(Swap<uint8_t>(0x00), 0x00);
}

TEST(EndianUtilsTest, SwapUInt16) {
    EXPECT_EQ(Swap<uint16_t>(0x1234), 0x3412);
    EXPECT_EQ(Swap<uint16_t>(0x0000), 0x0000);
    EXPECT_EQ(Swap<uint16_t>(0xFFFF), 0xFFFF);
}

TEST(EndianUtilsTest, SwapUInt32) {
    EXPECT_EQ(Swap<uint32_t>(0x12345678), 0x78563412);
    EXPECT_EQ(Swap<uint32_t>(0x00000000), 0x00000000);
    EXPECT_EQ(Swap<uint32_t>(0xFFFFFFFF), 0xFFFFFFFF);
}

TEST(EndianUtilsTest, SwapUInt64) {
    EXPECT_EQ(Swap<uint64_t>(0x123456789ABCDEF0), 0x34127856BC9AF0DE);
    EXPECT_EQ(Swap<uint64_t>(0x0000000000000000), 0x0000000000000000);
    EXPECT_EQ(Swap<uint64_t>(0xFFFFFFFFFFFFFFFF), 0xFFFFFFFFFFFFFFFF);
}

TEST(EndianUtilsTest, FromBigEndianToNative) {
    const uint16_t val16 = 0x1234;
    const uint32_t val32 = 0x12345678;
    const uint64_t val64 = 0x123456789ABCDEF0;

    if constexpr (std::endian::native == std::endian::little) {
        EXPECT_EQ(FromBigEndian(val16), Swap(val16));
        EXPECT_EQ(FromBigEndian(val32), Swap(val32));
        EXPECT_EQ(FromBigEndian(val64), Swap(val64));
    } else {
        EXPECT_EQ(FromBigEndian(val16), val16);
        EXPECT_EQ(FromBigEndian(val32), val32);
        EXPECT_EQ(FromBigEndian(val64), val64);
    }
}

TEST(EndianUtilsTest, FromLittleEndianToNative) {
    const uint16_t val16 = 0x3412;
    const uint32_t val32 = 0x78563412;
    const uint64_t val64 = 0xF0DEBC9A78563412;

    if constexpr (std::endian::native == std::endian::little) {
        EXPECT_EQ(FromLittleEndian(val16), val16);
        EXPECT_EQ(FromLittleEndian(val32), val32);
        EXPECT_EQ(FromLittleEndian(val64), val64);
    } else {
        EXPECT_EQ(FromLittleEndian(val16), Swap(val16));
        EXPECT_EQ(FromLittleEndian(val32), Swap(val32));
        EXPECT_EQ(FromLittleEndian(val64), Swap(val64));
    }
}

}  // namespace
