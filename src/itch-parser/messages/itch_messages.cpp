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

#include "itch-parser/messages/itch_messages.h"

#include <array>
#include <cstdint>
#include <ostream>
#include <variant>

namespace ITCH {

auto InternalPrintMessage(std::ostream& out, const SystemEventMessage& msg) {
    out << "System Event:\n"
        << "  Timestamp: " << ArrayToUint48(msg.timestamp) << "\n"
        << "  Event Code: " << msg.event_code;
}

auto InternalPrintMessage(std::ostream& out, const StockTradingActionMessage& msg) {
    out << "Stock Trading Action:\n"
        << "  Timestamp: " << ArrayToUint48(msg.timestamp) << "\n"
        << "  Stock: " << ArrayToString<STOCK_LEN>(msg.stock) << "\n"
        << "  State: " << msg.trading_state;
}

auto InternalPrintMessage(std::ostream& out, const RegSHOMessage& msg) {
    out << "Reg SHO Message:\n"
        << "  Timestamp: " << ArrayToUint48(msg.timestamp) << "\n"
        << "  Stock: " << ArrayToString<STOCK_LEN>(msg.stock) << "\n"
        << "  Reg SHO Action: " << msg.reg_sho_action;
}

auto InternalPrintMessage(std::ostream& out, const MarketParticipantPositionMessage& msg) {
    out << "Market Participant Position:\n"
        << "  Timestamp: " << ArrayToUint48(msg.timestamp) << "\n"
        << "  MPID: " << ArrayToString<4>(msg.mpid) << "\n"
        << "  Stock: " << ArrayToString<STOCK_LEN>(msg.stock) << "\n"
        << "  Market Marker: " << msg.primary_market_maker << "\n"
        << "  Market Makder Mode: " << msg.market_maker_mode << "\n"
        << "  Participant State: " << msg.market_participant_state;
}

auto InternalPrintMessage(std::ostream& out, const MWCBDeclineLevelMessage& msg) {
    out << "MWCB Decline Level:\n"
        << "  Timestamp: " << ArrayToUint48(msg.timestamp) << "\n"
        << "  Level 1: " << static_cast<double>(msg.level1) / MWCB_PRICE_DIVISOR
        << "  Level 2: " << static_cast<double>(msg.level2) / MWCB_PRICE_DIVISOR
        << "  Level 3: " << static_cast<double>(msg.level3) / MWCB_PRICE_DIVISOR;
}

auto InternalPrintMessage(std::ostream& out, const MWCBStatusMessage& msg) {
    out << "MWCB Status:\n"
        << "  Timestamp: " << ArrayToUint48(msg.timestamp) << "\n"
        << "  Breached Level: " << msg.breached_level;
}

auto InternalPrintMessage(std::ostream& out, const StockDirectoryMessage& msg) {
    out << "Stock Directory:\n"
        << "  Timestamp: " << ArrayToUint48(msg.timestamp) << "\n"
        << "  Stock: " << ArrayToString<STOCK_LEN>(msg.stock) << "\n"
        << "  Market Category: " << msg.market_category << "\n"
        << "  FinancialStatus Indicator: " << msg.etp_leverage_factor << "\n"
        << "  Round Lot Size: " << msg.round_lot_size << "\n"
        << "  Stock: " << ArrayToString<STOCK_LEN>(msg.stock) << "\n"
        << "  Round Lots Only: " << msg.round_lots_only << "\n"
        << "  Issue Classification: " << msg.issue_classification << "\n"
        << "  Issue Sub Type: " << ArrayToString<2>(msg.issue_sub_type) << "\n"
        << "  Authenticity: " << msg.authenticity << "\n"
        << "  Short Sale Threshold Indicator: " << msg.short_sale_threshold_indicator << "\n"
        << "  IPO Flag: " << msg.ipo_flag << "\n"
        << "  LULDReference Price Tier: " << msg.luld_ref << "\n"
        << "  ETP Flag: " << msg.etp_flag << "\n"
        << "  ETP Leverage Factor: " << msg.etp_leverage_factor << "\n"
        << "  Inverse Indicator: " << msg.inverse_indicator << "\n";
}

auto InternalPrintMessage(std::ostream& out, const IPOQuotingPeriodUpdateMessage& msg) {
    out << "IPO Quoting Period Update:\n"
        << "  Timestamp: " << ArrayToUint48(msg.timestamp) << "\n"
        << "  Stock: " << ArrayToString<STOCK_LEN>(msg.stock) << "\n"
        << "  IPO Quotation Release Time: " << msg.ipo_quotation_release_time << "\n"
        << "  IPO Quotation Release Qualifier: " << msg.ipo_quotation_release_qualifier << "\n"
        << "  IPO Price: " << msg.ipo_price << "\n";
}

auto InternalPrintMessage(std::ostream& out, const LULDAuctionCollarMessage& msg) {
    out << "LULD Auction Collar:\n"
        << "  Timestamp: " << ArrayToUint48(msg.timestamp) << "\n"
        << "  Stock: " << ArrayToString<STOCK_LEN>(msg.stock) << "\n"
        << "  Auction Collar Reference Price: " << msg.auction_collar_reference_price << "\n"
        << "  Upper Auction Collar Price: " << msg.upper_auction_collar_price << "\n"
        << "  Lower Auction Collar Price: " << msg.lower_auction_collar_price << "\n"
        << "  Auction Collar Extension: " << msg.auction_collar_extension;
}

auto InternalPrintMessage(std::ostream& out, const OperationalHaltMessage& msg) {
    out << "Operational Halt:\n"
        << "  Timestamp: " << ArrayToUint48(msg.timestamp) << "\n"
        << "  Stock: " << ArrayToString<STOCK_LEN>(msg.stock) << "\n"
        << "  Market Code: " << msg.market_code << "\n"
        << "  Operational Halt Action: " << msg.operational_halt_action;
}

auto InternalPrintMessage(std::ostream& out, const AddOrderMessage& msg) {
    out << "Add Order:\n"
        << "  Timestamp: " << ArrayToUint48(msg.timestamp) << "\n"
        << "  Order Reference ID: " << msg.order_reference_number << "\n"
        << "  Side: " << msg.buy_sell_indicator << "\n"
        << "  Shares: " << msg.shares << "\n"
        << "  Stock: " << ArrayToString<STOCK_LEN>(msg.stock) << "\n"
        << "  Price: " << msg.price / PRICE_DIVISOR;
}

auto InternalPrintMessage(std::ostream& out, const AddOrderMPIDAttributionMessage& msg) {
    out << "Add Order (MPID):\n"
        << "  Timestamp: " << ArrayToUint48(msg.timestamp) << "\n"
        << "  Order Reference ID: " << msg.order_reference_number << "\n"
        << "  Side: " << msg.buy_sell_indicator << "\n"
        << "  Shares: " << msg.shares << "\n"
        << "  Stock: " << ArrayToString<STOCK_LEN>(msg.stock) << "\n"
        << "  Price: " << (msg.price / PRICE_DIVISOR) << "\n"
        << "  Market Sarticipant ID: " << ArrayToString<4>(msg.attribution);
}

auto InternalPrintMessage(std::ostream& out, const OrderExecutedMessage& msg) {
    // out << "Order Executed:\n"
    //     << "  Timestamp: " << msg.header.timestamp << "\n"
    //     << "  Ref#: " << msg.order_reference_number << "\n"
    //     << "  Shares: " << msg.executed_shares;
}

auto InternalPrintMessage(std::ostream& out, const OrderExecutedWithPriceMessage& msg) {
    // out << "Order Executed w/ Price:\n"
    //     << "  Timestamp: " << msg.header.timestamp << "\n"
    //     << "  Ref#: " << msg.order_reference_number << "\n"
    //     << "  Price: " << msg.execution_price / PRICE_DIVISOR;
}

auto InternalPrintMessage(std::ostream& out, const OrderCancelMessage& msg) {
    // out << "Order Cancel:\n"
    //     << "  Timestamp: " << msg.header.timestamp << "\n"
    //     << "  Ref#: " << msg.order_reference_number << "\n"
    //     << "  Cancelled Shares: " << msg.cancelled_shares;
}

auto InternalPrintMessage(std::ostream& out, const OrderDeleteMessage& msg) {
    // out << "Order Delete:\n"
    //     << "  Timestamp: " << msg.header.timestamp << "\n"
    //     << "  Ref#: " << msg.order_reference_number;
}

auto InternalPrintMessage(std::ostream& out, const OrderReplaceMessage& msg) {
    // out << "Order Replace:\n"
    //     << "  Timestamp: " << msg.header.timestamp << "\n"
    //     << "  Original Ref#: " << msg.original_order_reference_number << "\n"
    //     << "  New Ref#: " << msg.new_order_reference_number << "\n"
    //     << "  Shares: " << msg.shares << "\n"
    //     << "  Price: " << msg.price / PRICE_DIVISOR;
}

auto InternalPrintMessage(std::ostream& out, const NonCrossTradeMessage& msg) {
    // out << "Non-Cross Trade:\n"
    //     << "  Timestamp: " << msg.header.timestamp << "\n"
    //     << "  Stock: " << to_string(msg.stock, STOCK_LEN) << "\n"
    //     << "  Side: " << msg.buy_sell_indicator << "\n"
    //     << "  Shares: " << msg.shares << "\n"
    //     << "  Price: " << msg.price / PRICE_DIVISOR;
}

auto InternalPrintMessage(std::ostream& out, const CrossTradeMessage& msg) {
    // out << "Cross Trade:\n"
    //     << "  Timestamp: " << msg.header.timestamp << "\n"
    //     << "  Stock: " << to_string(msg.stock, STOCK_LEN) << "\n"
    //     << "  Shares: " << msg.shares << "\n"
    //     << "  Cross Price: " << msg.cross_price / PRICE_DIVISOR << "\n"
    //     << "  Match#: " << msg.match_number << "\n"
    //     << "  Cross Type: " << msg.cross_type;
}

auto InternalPrintMessage(std::ostream& out, const BrokenTradeMessage& msg) {
    // out << "Broken Trade:\n"
    //     << "  Timestamp: " << msg.header.timestamp << "\n"
    //     << "  Match#: " << msg.match_number;
}

auto InternalPrintMessage(std::ostream& out, const NOIIMessage& msg) {
    // out << "NOII Message:\n"
    //     << "  Timestamp: " << msg.header.timestamp << "\n"
    //     << "  Stock: " << to_string(msg.stock, STOCK_LEN) << "\n"
    //     << "  Paired Shares: " << msg.paired_shares << "\n"
    //     << "  Imbalance Shares: " << msg.imbalance_shares << "\n"
    //     << "  Imbalance Direction: " << msg.imbalance_direction << "\n"
    //     << "  Far Price: " << msg.far_price / PRICE_DIVISOR << "\n"
    //     << "  Near Price: " << msg.near_price / PRICE_DIVISOR << "\n"
    //     << "  Reference Price: " << msg.current_reference_price / PRICE_DIVISOR << "\n"
    //     << "  Cross Type: " << msg.cross_type << "\n"
    //     << "  Price Variation Indicator: " << msg.price_variation_indicator;
}

auto InternalPrintMessage(std::ostream& out, const RetailPriceImprovementIndicatorMessage& msg) {
    // out << "RPII Message:\n"
    //     << "  Timestamp: " << msg.header.timestamp << "\n"
    //     << "  Stock: " << to_string(msg.stock, STOCK_LEN);
}

auto InternalPrintMessage(std::ostream& out, const DLCRMessage& msg) {
    // out << "DLCR Message:\n"
    //     << "  Timestamp: " << msg.header.timestamp << "\n"
    //     << "  Stock: " << to_string(msg.stock, STOCK_LEN);
}

auto PrintMessage(std::ostream& out, const Message& msg) -> void {
    std::visit([&out](auto&& arg) { InternalPrintMessage(out, arg); }, msg);
}

std::ostream& operator<<(std::ostream& out, const Message& msg) {
    PrintMessage(out, msg);
    return out;
}

uint64_t ArrayToUint48(const std::array<uint8_t, TIME_LEN>& bytes) {
    constexpr int first = 0;
    constexpr int second = 8;
    constexpr int third = 16;
    constexpr int fourth = 24;
    constexpr int fifth = 32;
    constexpr int sixth = 40;

    const uint64_t result = (static_cast<uint64_t>(bytes.at(0)) << sixth) |
                            (static_cast<uint64_t>(bytes.at(1)) << fifth) |
                            (static_cast<uint64_t>(bytes.at(2)) << fourth) |
                            (static_cast<uint64_t>(bytes.at(3)) << third) |
                            (static_cast<uint64_t>(bytes.at(4)) << second) |
                            (static_cast<uint64_t>(bytes.at(5)) << first);
    return result;
}

}  // namespace ITCH