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

#include "itch-parser/itch_parser.h"
#include "memory-map/memory_map_file.h"

class ITCHPaserTest : public ::testing::Test {
   protected:
    std::filesystem::path tempFilePath;
    std::string stringTempFilePath;

    void SetUp() override {
        // Create a temporary file path for testing
        auto tempDir = std::filesystem::temp_directory_path();
        tempFilePath = tempDir / "test_memory_map_data.bin";
        stringTempFilePath = tempFilePath.generic_string();
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

TEST_F(ITCHPaserTest, SystemEventMessageTest) {
    CreateTestFile({
        0x53,                                // Message Type ('S')
        0xCC, 0xBB,                          // Stock Locate (52411)
        0xFF, 0xAA,                          // Tracking Number (65450)
        0xAA, 0xBB, 0xCC, 0xAA, 0xBB, 0xCC,  // Timestamp (187723569347532)
        0x51                                 // Event Code ('Q')
    });

    ITCH_Parser paser(stringTempFilePath);
    ITCH::Message message = paser.DecodeMessage(ITCH::MessageType::SystemEventMessage);

    EXPECT_EQ(ITCH::MessageType::SystemEventMessage,
              std::get<ITCH::SystemEventMessage>(message).message_type);
    EXPECT_EQ(52411, std::get<ITCH::SystemEventMessage>(message).stock_locate);
    EXPECT_EQ(65450, std::get<ITCH::SystemEventMessage>(message).tracking_number);
    EXPECT_EQ(187723569347532, std::get<ITCH::SystemEventMessage>(message).timestamp);
    EXPECT_EQ('Q', std::get<ITCH::SystemEventMessage>(message).event_code);
}

TEST_F(ITCHPaserTest, StockTradingActionMessageTest) {
    CreateTestFile({
        0x48,                                            // Message Type ('H')
        0xFF, 0xAA,                                      // Stock Locate (65450)
        0xFF, 0xAA,                                      // Tracking Number (65450)
        0xAA, 0xBB, 0xCC, 0xAA, 0xBB, 0xCC,              // Timestamp (187723569347532)
        0x41, 0x42, 0x43, 0x44, 0x41, 0x42, 0x43, 0x44,  // Stock ID (ABCDABCD)
        0x50,                                            // Trading State ('P')
        0x30,                                            // Reserved ('0')
        0x41, 0x42, 0x43, 0x44                           // Reason (ABCD)
    });

    std::array<char, 8> expected_stock{'A', 'B', 'C', 'D', 'A', 'B', 'C', 'D'};
    std::array<char, 4> expected_reason{'A', 'B', 'C', 'D'};

    ITCH_Parser paser(stringTempFilePath);
    ITCH::Message message = paser.DecodeMessage(ITCH::MessageType::StockTradingActionMessage);

    EXPECT_EQ(ITCH::MessageType::StockTradingActionMessage,
              std::get<ITCH::StockTradingActionMessage>(message).message_type);
    EXPECT_EQ(65450, std::get<ITCH::StockTradingActionMessage>(message).stock_locate);
    EXPECT_EQ(65450, std::get<ITCH::StockTradingActionMessage>(message).tracking_number);
    EXPECT_EQ(187723569347532, std::get<ITCH::StockTradingActionMessage>(message).timestamp);
    EXPECT_EQ(expected_stock, std::get<ITCH::StockTradingActionMessage>(message).stock);
    EXPECT_EQ('P', std::get<ITCH::StockTradingActionMessage>(message).trading_state);
    EXPECT_EQ('0', std::get<ITCH::StockTradingActionMessage>(message).reserved);
    EXPECT_EQ(expected_reason, std::get<ITCH::StockTradingActionMessage>(message).reason);
}

TEST_F(ITCHPaserTest, RegSHOMessageTest) {
    CreateTestFile({
        0x59,                                            // Message Type ('Y')
        0x00, 0x0A,                                      // Stock Locate (10)
        0x00, 0xFF,                                      // Tracking Number (255)
        0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,              // Timestamp (16777215)
        0x41, 0x50, 0x50, 0x4C, 0x20, 0x20, 0x20, 0x20,  // Stock ID ('APPL    ')
        0x31,                                            // Reg SHO Action ('1')
    });

    std::array<char, 8> expected_stock{'A', 'P', 'P', 'L', ' ', ' ', ' ', ' '};

    ITCH_Parser paser(stringTempFilePath);
    ITCH::Message message = paser.DecodeMessage(ITCH::MessageType::RegSHOMessage);

    EXPECT_EQ(ITCH::MessageType::RegSHOMessage,
              std::get<ITCH::RegSHOMessage>(message).message_type);
    EXPECT_EQ(10, std::get<ITCH::RegSHOMessage>(message).stock_locate);
    EXPECT_EQ(255, std::get<ITCH::RegSHOMessage>(message).tracking_number);
    EXPECT_EQ(16777215, std::get<ITCH::RegSHOMessage>(message).timestamp);
    EXPECT_EQ(expected_stock, std::get<ITCH::RegSHOMessage>(message).stock);
    EXPECT_EQ('1', std::get<ITCH::RegSHOMessage>(message).reg_sho_action);
}

TEST_F(ITCHPaserTest, MarketParticipantPositionMessageTest) {
    CreateTestFile({
        0x4C,                                            // Message Type ('L')
        0x00, 0x0A,                                      // Stock Locate (10)
        0x00, 0xFF,                                      // Tracking Number (255)
        0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,              // Timestamp (16777215)
        0x57, 0x58, 0x59, 0x5A,                          // MPID (WXYZ)
        0x4E, 0x56, 0x44, 0x41, 0x20, 0x20, 0x20, 0x20,  // Stock ID ('NVDA    ')
        0x4E,                                            // Market Maker ('N')
        0x52,                                            // Marker Mode ('R')
        0x45                                             // State ('E')
    });

    std::array<char, 8> expected_stock{'N', 'V', 'D', 'A', ' ', ' ', ' ', ' '};
    std::array<char, 4> expected_mpid{'W', 'X', 'Y', 'Z'};

    ITCH_Parser paser(stringTempFilePath);
    ITCH::Message message =
        paser.DecodeMessage(ITCH::MessageType::MarketParticipantPositionMessage);

    EXPECT_EQ(ITCH::MessageType::MarketParticipantPositionMessage,
              std::get<ITCH::MarketParticipantPositionMessage>(message).message_type);
    EXPECT_EQ(10, std::get<ITCH::MarketParticipantPositionMessage>(message).stock_locate);
    EXPECT_EQ(255, std::get<ITCH::MarketParticipantPositionMessage>(message).tracking_number);
    EXPECT_EQ(16777215, std::get<ITCH::MarketParticipantPositionMessage>(message).timestamp);
    EXPECT_EQ(expected_stock, std::get<ITCH::MarketParticipantPositionMessage>(message).stock);
    EXPECT_EQ(expected_mpid, std::get<ITCH::MarketParticipantPositionMessage>(message).mpid);
    EXPECT_EQ('N', std::get<ITCH::MarketParticipantPositionMessage>(message).primary_market_maker);
    EXPECT_EQ('R', std::get<ITCH::MarketParticipantPositionMessage>(message).market_maker_mode);
    EXPECT_EQ('E',
              std::get<ITCH::MarketParticipantPositionMessage>(message).market_participant_state);
}
// NOLINTEND