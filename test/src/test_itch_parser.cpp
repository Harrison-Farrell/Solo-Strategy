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
#include "itch-parser/messages/itch_messages.h"
#include "memory-map/memory_map_file.h"
#include "utilities/endian_utils.h"

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
    using namespace std;
    using namespace endian;
    using namespace ITCH;

    CreateTestFile({
        0x53,                                // Message Type ('S')
        0xCC, 0xBB,                          // Stock Locate (52411)
        0xFF, 0xAA,                          // Tracking Number (65450)
        0x41, 0x41, 0x41, 0x41, 0x41, 0x41,  // Timestamp ('A' 'A' 'A' 'A' 'A' 'A')
        0x51                                 // Event Code ('Q')
    });

    array<uint8_t, 6> expected_timestamp{'A', 'A', 'A', 'A', 'A', 'A'};

    ITCH_Parser paser(stringTempFilePath);
    Message message = paser.DecodeMessage();

    EXPECT_EQ(MessageType::SystemEventMessage, get<SystemEventMessage>(message).message_type);
    EXPECT_EQ(52411, FromBigEndian(get<SystemEventMessage>(message).stock_locate));
    EXPECT_EQ(65450, FromBigEndian(get<SystemEventMessage>(message).tracking_number));
    EXPECT_EQ(expected_timestamp, get<SystemEventMessage>(message).timestamp);
    EXPECT_EQ('Q', get<SystemEventMessage>(message).event_code);
}

TEST_F(ITCHPaserTest, StockTradingActionMessageTest) {
    using namespace std;
    using namespace endian;
    using namespace ITCH;

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

    array<uint8_t, 6> expected_timestamp{0xAA, 0xBB, 0xCC, 0xAA, 0xBB, 0xCC};
    array<char, 8> expected_stock{'A', 'B', 'C', 'D', 'A', 'B', 'C', 'D'};
    array<char, 4> expected_reason{'A', 'B', 'C', 'D'};

    ITCH_Parser paser(stringTempFilePath);
    Message message = paser.DecodeMessage();

    EXPECT_EQ(MessageType::StockTradingActionMessage,
              get<StockTradingActionMessage>(message).message_type);
    EXPECT_EQ(65450, FromBigEndian(get<StockTradingActionMessage>(message).stock_locate));
    EXPECT_EQ(65450, FromBigEndian(get<StockTradingActionMessage>(message).tracking_number));
    EXPECT_EQ(187723569347532, ArrayToUint48(get<StockTradingActionMessage>(message).timestamp));
    EXPECT_EQ(expected_stock, std::get<ITCH::StockTradingActionMessage>(message).stock);
    EXPECT_EQ('P', std::get<ITCH::StockTradingActionMessage>(message).trading_state);
    EXPECT_EQ('0', std::get<ITCH::StockTradingActionMessage>(message).reserved);
    EXPECT_EQ(expected_reason, std::get<ITCH::StockTradingActionMessage>(message).reason);
}

TEST_F(ITCHPaserTest, RegSHOMessageTest) {
    using namespace std;
    using namespace endian;
    using namespace ITCH;

    CreateTestFile({
        0x59,                                            // Message Type ('Y')
        0x00, 0x0A,                                      // Stock Locate (10)
        0x00, 0xFF,                                      // Tracking Number (255)
        0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,              // Timestamp (16777215)
        0x41, 0x50, 0x50, 0x4C, 0x20, 0x20, 0x20, 0x20,  // Stock ID ('APPL    ')
        0x31,                                            // Reg SHO Action ('1')
    });

    ITCH_Parser paser(stringTempFilePath);
    Message message = paser.DecodeMessage();

    EXPECT_EQ(MessageType::RegSHOMessage, get<RegSHOMessage>(message).message_type);
    EXPECT_EQ(10, FromBigEndian(get<RegSHOMessage>(message).stock_locate));
    EXPECT_EQ(255, FromBigEndian(get<RegSHOMessage>(message).tracking_number));
    EXPECT_EQ(16777215, ArrayToUint48(get<ITCH::RegSHOMessage>(message).timestamp));
    EXPECT_EQ("APPL    ", ArrayToString<8>(get<RegSHOMessage>(message).stock));
    EXPECT_EQ('1', get<RegSHOMessage>(message).reg_sho_action);
}

TEST_F(ITCHPaserTest, MarketParticipantPositionMessageTest) {
    using namespace std;
    using namespace endian;
    using namespace ITCH;

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

    ITCH_Parser paser(stringTempFilePath);
    Message message = paser.DecodeMessage();

    EXPECT_EQ(MessageType::MarketParticipantPositionMessage,
              get<MarketParticipantPositionMessage>(message).message_type);
    EXPECT_EQ(10, FromBigEndian(get<MarketParticipantPositionMessage>(message).stock_locate));
    EXPECT_EQ(255, FromBigEndian(get<MarketParticipantPositionMessage>(message).tracking_number));
    EXPECT_EQ(16777215, ArrayToUint48(get<MarketParticipantPositionMessage>(message).timestamp));
    EXPECT_EQ("NVDA    ", ArrayToString<8>(get<MarketParticipantPositionMessage>(message).stock));
    EXPECT_EQ("WXYZ", ArrayToString<4>(get<MarketParticipantPositionMessage>(message).mpid));
    EXPECT_EQ('N', get<MarketParticipantPositionMessage>(message).primary_market_maker);
    EXPECT_EQ('R', get<MarketParticipantPositionMessage>(message).market_maker_mode);
    EXPECT_EQ('E', get<MarketParticipantPositionMessage>(message).market_participant_state);
}

TEST_F(ITCHPaserTest, MWCBDeclineLevelMessageTest) {
    using namespace std;
    using namespace endian;
    using namespace ITCH;

    CreateTestFile({
        0x56,                                            // Message Type ('V')
        0x00, 0x00,                                      // Stock Locate (0)
        0x00, 0xFF,                                      // Tracking Number (255)
        0xBC, 0xCD, 0x00, 0xFF, 0xFF, 0xFF,              // Timestamp (207588671094783)
        0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00,  // Level 1 (280375465148160)
        0x00, 0xAA, 0xFF, 0xBB, 0x00, 0xAA, 0xFF, 0xBB,  // Level 2 (48131924675985339)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01   // Level 3 (1)
    });

    ITCH_Parser paser(stringTempFilePath);
    Message message = paser.DecodeMessage();

    EXPECT_EQ(MessageType::MWCBDeclineLevelMessage,
              get<MWCBDeclineLevelMessage>(message).message_type);

    EXPECT_EQ(0, FromBigEndian(get<MWCBDeclineLevelMessage>(message).stock_locate));
    EXPECT_EQ(255, FromBigEndian(get<MWCBDeclineLevelMessage>(message).tracking_number));
    EXPECT_EQ(207588671094783, ArrayToUint48(get<MWCBDeclineLevelMessage>(message).timestamp));
    EXPECT_EQ(280375465148160, FromBigEndian(get<MWCBDeclineLevelMessage>(message).level1));
    EXPECT_EQ(48131924675985339, FromBigEndian(get<MWCBDeclineLevelMessage>(message).level2));
    EXPECT_EQ(1, FromBigEndian(get<MWCBDeclineLevelMessage>(message).level3));
}

TEST_F(ITCHPaserTest, MWCBStatusMessageTest) {
    using namespace std;
    using namespace endian;
    using namespace ITCH;

    CreateTestFile({
        0x57,                                // Message Type ('W')
        0x00, 0x00,                          // Stock Locate (0)
        0x00, 0xFF,                          // Tracking Number (255)
        0xBC, 0xCD, 0x00, 0xFF, 0xFF, 0xFF,  // Timestamp (207588671094783)
        0x33                                 // Breached Level ('3')
    });

    ITCH_Parser paser(stringTempFilePath);
    Message message = paser.DecodeMessage();

    EXPECT_EQ(MessageType::MWCBStatusMessage, get<MWCBStatusMessage>(message).message_type);

    EXPECT_EQ(0, FromBigEndian(get<MWCBStatusMessage>(message).stock_locate));
    EXPECT_EQ(255, FromBigEndian(get<MWCBStatusMessage>(message).tracking_number));
    EXPECT_EQ(207588671094783, ArrayToUint48(get<MWCBStatusMessage>(message).timestamp));
    EXPECT_EQ('3', get<MWCBStatusMessage>(message).breached_level);
}

TEST_F(ITCHPaserTest, StockDirectoryMessageTest) {
    using namespace std;
    using namespace endian;
    using namespace ITCH;

    CreateTestFile({
        0x52,                                            // Message Type
        0x00, 0x0D,                                      // Stock Locate
        0x00, 0x00,                                      // Tracking Numbe
        0x0A, 0x37, 0xD4, 0xD0, 0xD3, 0x09,              // Timestamp
        0x41, 0x50, 0x50, 0x4C, 0x20, 0x20, 0x20, 0x20,  // Stock
        0x51,                                            // Market Category
        0x4E,                                            // FinancialStatus Indicator
        0x00, 0x00, 0x00, 0x64,                          // Round Lot Size
        0x4E,                                            // Round Lots Only
        0x43,                                            // Issue Classification
        0x5A, 0x20,                                      // Issue Subtype
        0x50,                                            // Authenticity
        0x4E,                                            // Short Sale Threshold Indicator
        0x4E,                                            // IPO Flag
        0x31,                                            // LULDReference Price Tier
        0x4E,                                            // ETP Flag
        0x00, 0x00, 0x00, 0x00,                          // ETP Leverage Factor
        0x4E                                             // Inverse Indicator
    });

    ITCH_Parser paser(stringTempFilePath);
    Message message = paser.DecodeMessage();

    EXPECT_EQ(MessageType::StockDirectoryMessage, get<StockDirectoryMessage>(message).message_type);
    EXPECT_EQ(13, FromBigEndian(get<StockDirectoryMessage>(message).stock_locate));
    EXPECT_EQ(0, FromBigEndian(get<StockDirectoryMessage>(message).tracking_number));
    EXPECT_EQ(11234909934345, ArrayToUint48(get<StockDirectoryMessage>(message).timestamp));
    EXPECT_EQ("APPL    ", ArrayToString(get<StockDirectoryMessage>(message).stock));
    EXPECT_EQ('Q', get<StockDirectoryMessage>(message).market_category);
    EXPECT_EQ('N', get<StockDirectoryMessage>(message).financial_status_indicator);
    EXPECT_EQ(100, FromBigEndian(get<StockDirectoryMessage>(message).round_lot_size));
    EXPECT_EQ('N', get<StockDirectoryMessage>(message).round_lots_only);
    EXPECT_EQ('C', get<StockDirectoryMessage>(message).issue_classification);
    EXPECT_EQ("Z ", ArrayToString<2>(get<StockDirectoryMessage>(message).issue_sub_type));
    EXPECT_EQ('P', get<StockDirectoryMessage>(message).authenticity);
    EXPECT_EQ('N', get<StockDirectoryMessage>(message).short_sale_threshold_indicator);
    EXPECT_EQ('N', get<StockDirectoryMessage>(message).ipo_flag);
    EXPECT_EQ('1', get<StockDirectoryMessage>(message).luld_ref);
    EXPECT_EQ('N', get<StockDirectoryMessage>(message).etp_flag);
    EXPECT_EQ(0, FromBigEndian(get<StockDirectoryMessage>(message).etp_leverage_factor));
    EXPECT_EQ('N', get<StockDirectoryMessage>(message).inverse_indicator);
}

TEST_F(ITCHPaserTest, IPOQuotingPeriodUpdateMessageTest) {
    using namespace std;
    using namespace endian;
    using namespace ITCH;

    CreateTestFile({
        0x4B,                                            // Message Type
        0x00, 0x0D,                                      // Stock Locate
        0x00, 0x00,                                      // Tracking Numbe
        0x0A, 0x37, 0xD4, 0xD0, 0xD3, 0x09,              // Timestamp
        0x41, 0x50, 0x50, 0x4C, 0x20, 0x20, 0x20, 0x20,  // Stock
        0x00, 0x00, 0x00, 0x64,                          // IPO Quotation Release Time
        0x43,                                            // IPO Quotation Release Qualifier
        0x00, 0x64, 0x00, 0x00                           // IPO Price
    });

    ITCH_Parser paser(stringTempFilePath);
    Message message = paser.DecodeMessage();

    EXPECT_EQ(MessageType::IPOQuotingPeriodUpdateMessage,
              get<IPOQuotingPeriodUpdateMessage>(message).message_type);
    EXPECT_EQ(13, FromBigEndian(get<IPOQuotingPeriodUpdateMessage>(message).stock_locate));
    EXPECT_EQ(0, FromBigEndian(get<IPOQuotingPeriodUpdateMessage>(message).tracking_number));
    EXPECT_EQ(11234909934345, ArrayToUint48(get<IPOQuotingPeriodUpdateMessage>(message).timestamp));
    EXPECT_EQ("APPL    ", ArrayToString(get<IPOQuotingPeriodUpdateMessage>(message).stock));
    EXPECT_EQ(
        100, FromBigEndian(get<IPOQuotingPeriodUpdateMessage>(message).ipo_quotation_release_time));
    EXPECT_EQ('C', get<IPOQuotingPeriodUpdateMessage>(message).ipo_quotation_release_qualifier);
    EXPECT_EQ(6553600, FromBigEndian(get<IPOQuotingPeriodUpdateMessage>(message).ipo_price));
}

TEST_F(ITCHPaserTest, LULDAuctionCollarMessageTest) {
    using namespace std;
    using namespace endian;
    using namespace ITCH;

    CreateTestFile({
        0x4A,                                            // Message Type
        0x00, 0x0D,                                      // Stock Locate
        0x00, 0x00,                                      // Tracking Numbe
        0x0A, 0x37, 0xD4, 0xD0, 0xD3, 0x09,              // Timestamp
        0x41, 0x50, 0x50, 0x4C, 0x20, 0x20, 0x20, 0x20,  // Stock
        0x00, 0x0F, 0x42, 0x40,                          // Auction Collar Reference Price
        0x00, 0x10, 0x05, 0x90,                          // Upper Auction Collar Price
        0x00, 0x0D, 0xBB, 0xA0,                          // Lower Auction Collar Price
        0x00, 0x64, 0x00, 0x00                           // Auction Collar Extension
    });

    ITCH_Parser paser(stringTempFilePath);
    Message message = paser.DecodeMessage();

    EXPECT_EQ(MessageType::LULDAuctionCollarMessage,
              get<LULDAuctionCollarMessage>(message).message_type);
    EXPECT_EQ(13, FromBigEndian(get<LULDAuctionCollarMessage>(message).stock_locate));
    EXPECT_EQ(0, FromBigEndian(get<LULDAuctionCollarMessage>(message).tracking_number));
    EXPECT_EQ(11234909934345, ArrayToUint48(get<LULDAuctionCollarMessage>(message).timestamp));
    EXPECT_EQ("APPL    ", ArrayToString(get<LULDAuctionCollarMessage>(message).stock));
    EXPECT_EQ(1000000,
              FromBigEndian(get<LULDAuctionCollarMessage>(message).auction_collar_reference_price));
    EXPECT_EQ(1050000,
              FromBigEndian(get<LULDAuctionCollarMessage>(message).upper_auction_collar_price));
    EXPECT_EQ(900000,
              FromBigEndian(get<LULDAuctionCollarMessage>(message).lower_auction_collar_price));
    EXPECT_EQ(6553600,
              FromBigEndian(get<LULDAuctionCollarMessage>(message).auction_collar_extension));
}

TEST_F(ITCHPaserTest, OperationalHaltMessageTest) {
    using namespace std;
    using namespace endian;
    using namespace ITCH;

    CreateTestFile({
        0x68,                                            // Message Type
        0x00, 0x0D,                                      // Stock Locate
        0x00, 0x00,                                      // Tracking Numbe
        0x0A, 0x37, 0xD4, 0xD0, 0xD3, 0x09,              // Timestamp
        0x41, 0x50, 0x50, 0x4C, 0x20, 0x20, 0x20, 0x20,  // Stock
        0x51,                                            // Market Code
        0x48                                             // Operational Halt Action
    });

    ITCH_Parser paser(stringTempFilePath);
    Message message = paser.DecodeMessage();

    EXPECT_EQ(MessageType::OperationalHaltMessage,
              get<OperationalHaltMessage>(message).message_type);
    EXPECT_EQ(13, FromBigEndian(get<OperationalHaltMessage>(message).stock_locate));
    EXPECT_EQ(0, FromBigEndian(get<OperationalHaltMessage>(message).tracking_number));
    EXPECT_EQ(11234909934345, ArrayToUint48(get<OperationalHaltMessage>(message).timestamp));
    EXPECT_EQ("APPL    ", ArrayToString(get<OperationalHaltMessage>(message).stock));
    EXPECT_EQ('Q', get<OperationalHaltMessage>(message).market_code);
    EXPECT_EQ('H', get<OperationalHaltMessage>(message).operational_halt_action);
}

TEST_F(ITCHPaserTest, AddOrderMessageTest) {
    using namespace std;
    using namespace endian;
    using namespace ITCH;

    CreateTestFile({
        0x41,                                            // Message Type
        0x00, 0x0D,                                      // Stock Locate
        0x00, 0x00,                                      // Tracking Number
        0x0D, 0x1B, 0x51, 0x63, 0xB7, 0x25,              // Timestamp
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xA0, 0x15,  // Order Reference ID
        0x42,                                            // Buy / Side Indicator
        0x00, 0x00, 0x00, 0x64,                          // Shares
        0x41, 0x41, 0x50, 0x4C, 0x20, 0x20, 0x20, 0x20,  // Stock
        0x00, 0x2A, 0xB9, 0x80                           // Price
    });

    ITCH_Parser paser(stringTempFilePath);
    Message message = paser.DecodeMessage();

    EXPECT_EQ(MessageType::AddOrderMessage, get<AddOrderMessage>(message).message_type);
    EXPECT_EQ(13, FromBigEndian(get<AddOrderMessage>(message).stock_locate));
    EXPECT_EQ(0, FromBigEndian(get<AddOrderMessage>(message).tracking_number));
    EXPECT_EQ(14410980767525, ArrayToUint48(get<AddOrderMessage>(message).timestamp));
    EXPECT_EQ(106517, FromBigEndian(get<AddOrderMessage>(message).order_reference_number));
    EXPECT_EQ('B', get<AddOrderMessage>(message).buy_sell_indicator);
    EXPECT_EQ(100, FromBigEndian(get<AddOrderMessage>(message).shares));
    EXPECT_EQ("AAPL    ", ArrayToString(get<AddOrderMessage>(message).stock));
    EXPECT_EQ(2800000, FromBigEndian(get<AddOrderMessage>(message).price));
}
// NOLINTEND