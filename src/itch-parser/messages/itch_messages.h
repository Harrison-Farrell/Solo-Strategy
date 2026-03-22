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

/// \file itch_messages.h
/// \brief Structure definitions for Nasdaq TotalView ITCH 5.0 messages.
///
/// This file contains packed structures representing each message type in the
/// ITCH protocol. These structures can be directly mapped to memory-mapped
/// file contents for high-performance parsing.

#ifndef SOLO_STRATEGY_SRC_ITCH_PARSER_MESSAGES_ITCH_MESSAGES_H_
#define SOLO_STRATEGY_SRC_ITCH_PARSER_MESSAGES_ITCH_MESSAGES_H_

#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

namespace ITCH {

constexpr int STOCK_LEN = 8;
constexpr int TIME_LEN = 6;

/// \brief Divisor to convert 4-decimal fixed point prices to floating point.
constexpr double PRICE_DIVISOR = 10000.0;
/// \brief Divisor for MWCB messages which use 8-decimal precision.
constexpr double MWCB_PRICE_DIVISOR = 1.0E8;

using Timestamp = std::array<uint8_t, TIME_LEN>;
using StockID = std::array<char, STOCK_LEN>;

#pragma pack(push, 1)

enum class MessageType : char {
    SystemEventMessage = 'S',                     ///< 5 fields, 12 bytes
    StockDirectoryMessage = 'R',                  ///< 18 fields, 39 bytes
    StockTradingActionMessage = 'H',              ///< 8 fields, 25 bytes
    RegSHOMessage = 'Y',                          ///< 6 fields, 20 bytes
    MarketParticipantPositionMessage = 'L',       ///< 9 fields, 26 bytes
    MWCBDeclineLevelMessage = 'V',                ///< 7 fields, 35 bytes
    MWCBStatusMessage = 'W',                      ///< 5 fields, 12 bytes
    IPOQuotingPeriodUpdateMessage = 'K',          ///< 8 fields, 28 bytes
    LULDAuctionCollarMessage = 'J',               ///< 9 fields, 35 bytes
    OperationalHaltMessage = 'h',                 ///< 7 fields, 21 bytes
    AddOrderMessage = 'A',                        ///< 9 fields, 36 bytes
    AddOrderMPIDAttributionMessage = 'F',         ///< 10 fields, 38 bytes
    OrderExecutedMessage = 'E',                   ///< 7 fields, 31 bytes
    OrderExecutedWithPriceMessage = 'C',          ///< 9 fields, 36 bytes
    OrderCancelMessage = 'X',                     ///< 6 fields, 23 bytes
    OrderDeleteMessage = 'D',                     ///< 5 fields, 19 bytes
    OrderReplaceMessage = 'U',                    ///< 8 fields, 35 bytes
    NonCrossTradeMessage = 'P',                   ///< 10 fields, 44 bytes
    CrossTradeMessage = 'Q',                      ///< 9 fields, 40 bytes
    BrokenTradeMessage = 'B',                     ///< 4 fields, 19 bytes
    NOIIMessage = 'I',                            ///< 13 fields, 50 bytes
    DLCRMessage = 'O',                            ///< 7 fields, 20 bytes
    RetailPriceImprovementIndicatorMessage = 'N'  ///< 12 fields, 48 bytes
};

/// \struct SystemEventMessage
/// \brief Disseminated for various system-level events like start/end of day.
struct SystemEventMessage {
    MessageType message_type;  ///< Message type
    uint16_t stock_locate;     ///< Locate code for the security
    uint16_t tracking_number;  ///< Nasdaq internal tracking number
    Timestamp timestamp;       ///< Nanoseconds past midnight (48-bit)
    char event_code;           ///< Code for the system event ('O','S','Q','M','E','C');
};

/// \struct StockTradingActionMessage
/// \brief Current trading state of a security.
struct StockTradingActionMessage {
    MessageType message_type;    ///< Message type
    uint16_t stock_locate;       ///< Locate code for the security
    uint16_t tracking_number;    ///< Nasdaq internal tracking number
    Timestamp timestamp;         ///< Nanoseconds past midnight (48-bit) header
    StockID stock;               ///< Stock symbol
    char trading_state;          ///< Current trading state (e.g., 'H', 'T', 'Q')
    char reserved;               ///< Reserved field
    std::array<char, 4> reason;  ///< Reason for state change
};

/// \struct RegSHOMessage
/// \brief Disseminated when a Short Sale Price Test is in effect.
struct RegSHOMessage {
    MessageType message_type;  ///< Message type
    uint16_t stock_locate;     ///< Locate code for the security
    uint16_t tracking_number;  ///< Nasdaq internal tracking number
    Timestamp timestamp;       ///< Nanoseconds past midnight (48-bit)
    StockID stock;             ///< Stock symbol
    char reg_sho_action;       ///< Current SHO action ('0', '1', '2')
};

/// \struct MarketParticipantPositionMessage
/// \brief Quote information for a market participant.
struct MarketParticipantPositionMessage {
    MessageType message_type;       ///< Message type
    uint16_t stock_locate;          ///< Locate code for the security
    uint16_t tracking_number;       ///< Nasdaq internal tracking number
    Timestamp timestamp;            ///< Nanoseconds past midnight (48-bit)
    std::array<char, 4> mpid;       ///< Market Participant ID
    StockID stock;                  ///< Stock symbol
    char primary_market_maker;      ///< 'Y' if primary MM
    char market_maker_mode;         ///< Normal, Passive, etc.
    char market_participant_state;  ///< Active, Excused, etc.
};

/// \struct MWCBDeclineLevelMessage
/// \brief Market Wide Circuit Breaker decline levels.
struct MWCBDeclineLevelMessage {
    MessageType message_type;  ///< Message type
    uint16_t stock_locate;     ///< Locate code for the security
    uint16_t tracking_number;  ///< Nasdaq internal tracking number
    Timestamp timestamp;       ///< Nanoseconds past midnight (48-bit)
    uint64_t level1;           ///< Level 1 decline threshold (8 decimals)
    uint64_t level2;           ///< Level 2 decline threshold (8 decimals)
    uint64_t level3;           ///< Level 3 decline threshold (8 decimals)
};

/// \struct MWCBStatusMessage
/// \brief Current status of MWCB levels.
struct MWCBStatusMessage {
    MessageType message_type;  ///< Message type
    uint16_t stock_locate;     ///< Locate code for the security
    uint16_t tracking_number;  ///< Nasdaq internal tracking number
    Timestamp timestamp;       ///< Nanoseconds past midnight (48-bit)
    char breached_level;       ///< '1', '2', or '3' if breached
};

/// \struct StockDirectoryMessage
/// \brief Basic data for a stock, sent at the start of the day.
struct StockDirectoryMessage {
    MessageType message_type;             ///< Message type
    uint16_t stock_locate;                ///< Locate code for the security
    uint16_t tracking_number;             ///< Nasdaq internal tracking number
    Timestamp timestamp;                  ///< Nanoseconds past midnight (48-bit)
    StockID stock;                        ///< Stock symbol, right-padded with spaces
    char market_category;                 ///< Listing market category
    char financial_status_indicator;      ///< Financial status of the issuer
    uint32_t round_lot_size;              ///< Standard round lot size
    char round_lots_only;                 ///< 'Y' if only round lots are allowed
    char issue_classification;            ///< Type of issue
    std::array<char, 2> issue_sub_type;   ///< More granular issue type
    char authenticity;                    ///< 'P' for production, 'T' for test
    char short_sale_threshold_indicator;  ///< Threshold status
    char ipo_flag;                        ///< 'Y' if new IPO
    char luld_ref;                        ///< LULD reference price tier
    char etp_flag;                        ///< 'Y' if ETP
    uint32_t etp_leverage_factor;         ///< Leverage factor (8 decimals)
    char inverse_indicator;               ///< 'Y' if inverse ETP
};

/// \struct IPOQuotingPeriodUpdateMessage
/// \brief IPO quoting period tracking.
struct IPOQuotingPeriodUpdateMessage {
    MessageType message_type;              ///< Message type
    uint16_t stock_locate;                 ///< Locate code for the security
    uint16_t tracking_number;              ///< Nasdaq internal tracking number
    Timestamp timestamp;                   ///< Nanoseconds past midnight (48-bit)
    StockID stock;                         ///< Stock symbol
    uint32_t ipo_quotation_release_time;   ///< Seconds past midnight
    char ipo_quotation_release_qualifier;  ///< 'A' for anticipated, 'C' for cancelled
    uint32_t ipo_price;                    ///< IPO price (4 decimals)
};

/// \struct LULDAuctionCollarMessage
/// \brief Auction collar levels for LULD.
struct LULDAuctionCollarMessage {
    MessageType message_type;                 ///< Message type
    uint16_t stock_locate;                    ///< Locate code for the security
    uint16_t tracking_number;                 ///< Nasdaq internal tracking number
    Timestamp timestamp;                      ///< Nanoseconds past midnight (48-bit)
    StockID stock;                            ///< Stock symbol
    uint32_t auction_collar_reference_price;  ///< Reference price (4 decimals)
    uint32_t upper_auction_collar_price;      ///< Upper collar limit (4 decimals)
    uint32_t lower_auction_collar_price;      ///< Lower collar limit (4 decimals)
    uint32_t auction_collar_extension;        ///< Extension number
};

/// \struct OperationalHaltMessage
/// \brief Operational halt status per market.
struct OperationalHaltMessage {
    MessageType message_type;      ///< Message type
    uint16_t stock_locate;         ///< Locate code for the security
    uint16_t tracking_number;      ///< Nasdaq internal tracking number
    Timestamp timestamp;           ///< Nanoseconds past midnight (48-bit)
    StockID stock;                 ///< Stock symbol
    char market_code;              ///< 'Q', 'B', or 'X'
    char operational_halt_action;  ///< 'H' for halt, 'T' for trading
};

/// \struct AddOrderMessage
/// \brief New order added without attribution.
struct AddOrderMessage {
    MessageType message_type;         ///< Message type
    uint16_t stock_locate;            ///< Locate code for the security
    uint16_t tracking_number;         ///< Nasdaq internal tracking number
    Timestamp timestamp;              ///< Nanoseconds past midnight (48-bit)
    uint64_t order_reference_number;  ///< Unique order ID
    char buy_sell_indicator;          ///< 'B' for buy, 'S' for sell
    uint32_t shares;                  ///< Number of shares
    StockID stock;                    ///< Stock symbol
    uint32_t price;                   ///< Limit price (4 decimals)
};
//==============================================================================
/*
 * \struct MessageHeader
 * \brief Message header for all ITCH messages.
 */
struct MessageHeader {
    MessageType message_type;  ///< Message type
    uint16_t stock_locate;     ///< Locate code for the security
    uint16_t tracking_number;  ///< Nasdaq internal tracking number
    uint64_t timestamp;        ///< Nanoseconds past midnight (48-bit)
};

/// \struct AddOrderMPIDAttributionMessage
/// \brief New order added with MPID attribution.
struct AddOrderMPIDAttributionMessage {
    MessageHeader header{MessageType::AddOrderMPIDAttributionMessage, 0, 0, 0};  ///< Message header
    uint64_t order_reference_number;  ///< Unique order ID
    char buy_sell_indicator;          ///< 'B' for buy, 'S' for sell
    uint32_t shares;                  ///< Number of shares
    char stock[8];                    ///< Stock symbol
    uint32_t price;                   ///< Limit price (4 decimals)
    char attribution[4];              ///< Market Participant ID
};

/// \struct OrderExecutedMessage
/// \brief Full or partial execution of an existing order.
struct OrderExecutedMessage {
    MessageHeader header{MessageType::OrderExecutedMessage, 0, 0, 0};  ///< Message header
    uint64_t order_reference_number;                                   ///< Reference to AddOrder
    uint32_t executed_shares;                                          ///< Executed share count
    uint64_t match_number;                                             ///< Execution ID
};

/// \struct OrderExecutedWithPriceMessage
/// \brief Execution with a non-limit price (e.g., cross).
struct OrderExecutedWithPriceMessage {
    MessageHeader header{MessageType::OrderExecutedWithPriceMessage, 0, 0, 0};  ///< Message header
    uint64_t order_reference_number;  ///< Reference to AddOrder
    uint32_t executed_shares;         ///< Executed share count
    uint64_t match_number;            ///< Execution ID
    char printable;                   ///< 'Y' if searchable in trade reports
    uint32_t execution_price;         ///< Execution price (4 decimals)
};

/// \struct OrderCancelMessage
/// \brief Partial reduction of an order's shares.
struct OrderCancelMessage {
    MessageHeader header{MessageType::OrderCancelMessage, 0, 0, 0};  ///< Message header
    uint64_t order_reference_number;                                 ///< Reference to AddOrder
    uint32_t cancelled_shares;                                       ///< Amount reduced
};

/// \struct OrderDeleteMessage
/// \brief Removal of an order from the book.
struct OrderDeleteMessage {
    MessageHeader header{MessageType::OrderDeleteMessage, 0, 0, 0};  ///< Message header
    uint64_t order_reference_number;                                 ///< Reference to AddOrder
};

/// \struct OrderReplaceMessage
/// \brief Modification of an order's price or shares.
struct OrderReplaceMessage {
    MessageHeader header{MessageType::OrderReplaceMessage, 0, 0, 0};  ///< Message header
    uint64_t original_order_reference_number;                         ///< Old ID
    uint64_t new_order_reference_number;                              ///< New ID
    uint32_t shares;                                                  ///< New total shares
    uint32_t price;                                                   ///< New limit price
};

/// \struct NonCrossTradeMessage
/// \brief Trade result from hidden orders (standard trade).
struct NonCrossTradeMessage {
    MessageHeader header{MessageType::NonCrossTradeMessage, 0, 0, 0};  ///< Message header
    uint64_t order_reference_number;                                   ///< Internal ID
    char buy_sell_indicator;                                           ///< 'B' or 'S'
    uint32_t shares;                                                   ///< Size
    char stock[8];                                                     ///< Symbol
    uint32_t price;                                                    ///< Price
    uint64_t match_number;                                             ///< Execution ID
};

/// \struct CrossTradeMessage
/// \brief Opening, closing, or halt-cross trade result.
struct CrossTradeMessage {
    MessageHeader header{MessageType::CrossTradeMessage, 0, 0, 0};  ///< Message header
    uint64_t shares;                                                ///< Total shares crossed
    char stock[8];                                                  ///< Symbol
    uint32_t cross_price;                                           ///< Execution price
    uint64_t match_number;                                          ///< Execution ID
    char cross_type;                                                ///< 'O', 'C', 'H', or 'I'
};

/// \struct BrokenTradeMessage
/// \brief Indicates a previous trade was cancelled.
struct BrokenTradeMessage {
    MessageHeader header{MessageType::BrokenTradeMessage, 0, 0, 0};  ///< Message header
    uint64_t match_number;  ///< Match ID of the original trade
};

/// \struct NOIIMessage
/// \brief Net Order Imbalance Indicator.
struct NOIIMessage {
    MessageHeader header{MessageType::NOIIMessage, 0, 0, 0};  ///< Message header
    uint64_t paired_shares;            ///< Shares combined at reference price
    uint64_t imbalance_shares;         ///< Remaining shares
    char imbalance_direction;          ///< 'B', 'S', 'N', or 'O'
    char stock[8];                     ///< Symbol
    uint32_t far_price;                ///< Crossing price (4 decimals)
    uint32_t near_price;               ///< Crossing price (4 decimals)
    uint32_t current_reference_price;  ///< Market price (4 decimals)
    char cross_type;                   ///< 'O', 'C', or 'H'
    char price_variation_indicator;    ///< Degree of change
};

/// \struct RetailPriceImprovementIndicatorMessage
/// \brief RPI interest in a symbol.
struct RetailPriceImprovementIndicatorMessage {
    MessageHeader header{MessageType::RetailPriceImprovementIndicatorMessage, 0, 0,
                         0};  ///< Message header
    char stock[8];            ///< Symbol
    char interest_flag;       ///< 'B', 'S', 'A', or 'N'
};

/// \struct DLCRMessage
/// \brief Direct Listing Capital Raise information.
struct DLCRMessage {
    MessageHeader header{MessageType::DLCRMessage, 0, 0, 0};  ///< Message header
    char stock[8];                                            ///< Symbol
    char open_eligibility_status;                             ///< Verified/Unverified
    uint32_t minimum_allowable_price;                         ///< Minimum (4 decimals)
    uint32_t maximum_allowable_price;                         ///< Maximum (4 decimals)
    uint32_t near_execution_price;                            ///< Execution price (4 decimals)
    uint64_t near_execution_time;                             ///< Target time
    uint32_t lower_price_range_collar;                        ///< Floor (4 decimals)
    uint32_t upper_price_range_collar;                        ///< Ceiling (4 decimals)
};

#pragma pack(pop)

/// \typedef Message
/// \brief Variant type holding any specific ITCH message structure.
using Message =
    std::variant<SystemEventMessage, StockDirectoryMessage, StockTradingActionMessage,
                 RegSHOMessage, MarketParticipantPositionMessage, MWCBDeclineLevelMessage,
                 MWCBStatusMessage, IPOQuotingPeriodUpdateMessage, LULDAuctionCollarMessage,
                 OperationalHaltMessage, AddOrderMessage, AddOrderMPIDAttributionMessage,
                 OrderExecutedMessage, OrderExecutedWithPriceMessage, OrderCancelMessage,
                 OrderDeleteMessage, OrderReplaceMessage, NonCrossTradeMessage, CrossTradeMessage,
                 BrokenTradeMessage, NOIIMessage, RetailPriceImprovementIndicatorMessage,
                 DLCRMessage>;

template <size_t N>
std::string ArrayToString(const std::array<char, N>& arr) {
    // Uses the iterator constructor: std::string(begin, end)
    return std::string(arr.begin(), arr.end());
}

uint64_t ArrayToUint48(const std::array<uint8_t, TIME_LEN>& bytes);

auto InternalPrintMessage(std::ostream& out, const SystemEventMessage& msg);
auto InternalPrintMessage(std::ostream& out, const StockDirectoryMessage& msg);
auto InternalPrintMessage(std::ostream& out, const StockTradingActionMessage& msg);
auto InternalPrintMessage(std::ostream& out, const RegSHOMessage& msg);
auto InternalPrintMessage(std::ostream& out, const MarketParticipantPositionMessage& msg);
auto InternalPrintMessage(std::ostream& out, const MWCBDeclineLevelMessage& msg);
auto InternalPrintMessage(std::ostream& out, const MWCBStatusMessage& msg);
auto InternalPrintMessage(std::ostream& out, const IPOQuotingPeriodUpdateMessage& msg);
auto InternalPrintMessage(std::ostream& out, const LULDAuctionCollarMessage& msg);
auto InternalPrintMessage(std::ostream& out, const OperationalHaltMessage& msg);
auto InternalPrintMessage(std::ostream& out, const AddOrderMessage& msg);
auto InternalPrintMessage(std::ostream& out, const AddOrderMPIDAttributionMessage& msg);
auto InternalPrintMessage(std::ostream& out, const OrderExecutedMessage& msg);
auto InternalPrintMessage(std::ostream& out, const OrderExecutedWithPriceMessage& msg);
auto InternalPrintMessage(std::ostream& out, const OrderCancelMessage& msg);
auto InternalPrintMessage(std::ostream& out, const OrderDeleteMessage& msg);
auto InternalPrintMessage(std::ostream& out, const OrderReplaceMessage& msg);
auto InternalPrintMessage(std::ostream& out, const NonCrossTradeMessage& msg);
auto InternalPrintMessage(std::ostream& out, const CrossTradeMessage& msg);
auto InternalPrintMessage(std::ostream& out, const BrokenTradeMessage& msg);
auto InternalPrintMessage(std::ostream& out, const NOIIMessage& msg);
auto InternalPrintMessage(std::ostream& out, const RetailPriceImprovementIndicatorMessage& msg);
auto InternalPrintMessage(std::ostream& out, const DLCRMessage& msg);

/// \brief Formats a Message variant into a human-readable stream.
/// \param out Output stream.
/// \param msg Message variant.
auto PrintMessage(std::ostream& out, const Message& msg) -> void;

/// \brief Stream operator overload for ITCH messages.
auto operator<<(std::ostream& out, const Message& msg) -> std::ostream&;

}  // namespace ITCH

#endif  // SOLO_STRATEGY_SRC_ITCH_PARSER_MESSAGES_ITCH_MESSAGES_H_