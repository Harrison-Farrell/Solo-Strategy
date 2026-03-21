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

#include "itch-parser/itch_parser.h"

#include <string>

#include "itch-parser/messages/itch_messages.h"
#include "memory-map/memory_map_file.h"

auto ITCH_Parser::SystemEventMessage() {
    return ITCH::SystemEventMessage{.message_type = ITCH::MessageType::SystemEventMessage,
                                    .stock_locate = m_Reader.Read16(),
                                    .tracking_number = m_Reader.Read16(),
                                    .timestamp = m_Reader.Read48(),
                                    .event_code = m_Reader.ReadChar()};
}

auto ITCH_Parser::StockTradingActionMessage() {
    return ITCH::StockTradingActionMessage{
        .message_type = ITCH::MessageType::StockTradingActionMessage,
        .stock_locate = m_Reader.Read16(),
        .tracking_number = m_Reader.Read16(),
        .timestamp = m_Reader.Read48(),
        .stock = m_Reader.ReadSymbol(),
        .trading_state = m_Reader.ReadChar(),
        .reserved = m_Reader.ReadChar(),
        .reason = m_Reader.ReadArray<4>()};
}

auto ITCH_Parser::RegSHOMessage() {
    return ITCH::RegSHOMessage{.message_type = ITCH::MessageType::RegSHOMessage,
                               .stock_locate = m_Reader.Read16(),
                               .tracking_number = m_Reader.Read16(),
                               .timestamp = m_Reader.Read48(),
                               .stock = m_Reader.ReadSymbol(),
                               .reg_sho_action = m_Reader.ReadChar()};
}

auto ITCH_Parser::MarketParticipantPositionMessage() {
    return ITCH::MarketParticipantPositionMessage{
        .message_type = ITCH::MessageType::MarketParticipantPositionMessage,
        .stock_locate = m_Reader.Read16(),
        .tracking_number = m_Reader.Read16(),
        .timestamp = m_Reader.Read48(),
        .mpid = m_Reader.ReadArray<4>(),
        .stock = m_Reader.ReadSymbol(),
        .primary_market_maker = m_Reader.ReadChar(),
        .market_maker_mode = m_Reader.ReadChar(),
        .market_participant_state = m_Reader.ReadChar()};
}

ITCH_Parser::ITCH_Parser(const std::string& file_path)
    : m_Reader(file_path), m_MessageQueue(m_MessageQueueSize) {
    // clang-format off
    using namespace ITCH;
    m_dispatch[MessageType::SystemEventMessage] = [this](){return SystemEventMessage();};
    m_dispatch[MessageType::StockTradingActionMessage] = [this](){return StockTradingActionMessage();};
    m_dispatch[MessageType::RegSHOMessage] = [this](){return RegSHOMessage();};
    m_dispatch[MessageType::MarketParticipantPositionMessage] = [this](){return MarketParticipantPositionMessage();};
    // clang-format on
}

auto ITCH_Parser::StockDirectoryMessage(const ITCH::MessageHeader& header) {
    char symbols[4];
    m_Reader.copyString(symbols, 4);
    char financial_status_indicator = m_Reader.ReadChar();
    uint32_t round_lot_size = m_Reader.Read32();
    char round_lots_only = m_Reader.ReadChar();
    char issue_classification = m_Reader.ReadChar();
    char issue_sub_type[2];
    issue_sub_type[0] = m_Reader.ReadChar();
    issue_sub_type[1] = m_Reader.ReadChar();
    char authenticity = m_Reader.ReadChar();
    char short_sale_threshold_indicator = m_Reader.ReadChar();
    char ipo_flag = m_Reader.ReadChar();
    char luld_ref = m_Reader.ReadChar();
    char etp_flag = m_Reader.ReadChar();
    char etp_leverage_factor = m_Reader.Read32();
    char inverse_indicator = m_Reader.ReadChar();
}

auto ITCH_Parser::MWCBDeclineLevelMessage(const ITCH::MessageHeader& header) {
    ITCH::MWCBDeclineLevelMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::MWCBStatusMessage(const ITCH::MessageHeader& header) {
    ITCH::MWCBStatusMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::IPOQuotingPeriodUpdateMessage(const ITCH::MessageHeader& header) {
    ITCH::IPOQuotingPeriodUpdateMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::LULDAuctionCollarMessage(const ITCH::MessageHeader& header) {
    ITCH::LULDAuctionCollarMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::OperationalHaltMessage(const ITCH::MessageHeader& header) {
    ITCH::OperationalHaltMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::AddOrderMessage(const ITCH::MessageHeader& header) {
    ITCH::AddOrderMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::AddOrderMPIDAttributionMessage(const ITCH::MessageHeader& header) {
    ITCH::AddOrderMPIDAttributionMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::OrderExecutedMessage(const ITCH::MessageHeader& header) {
    ITCH::OrderExecutedMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::OrderExecutedWithPriceMessage(const ITCH::MessageHeader& header) {
    ITCH::OrderExecutedWithPriceMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::OrderCancelMessage(const ITCH::MessageHeader& header) {
    ITCH::OrderCancelMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::OrderDeleteMessage(const ITCH::MessageHeader& header) {
    ITCH::OrderDeleteMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::OrderReplaceMessage(const ITCH::MessageHeader& header) {
    ITCH::OrderReplaceMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::NonCrossTradeMessage(const ITCH::MessageHeader& header) {
    ITCH::NonCrossTradeMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::CrossTradeMessage(const ITCH::MessageHeader& header) {
    ITCH::CrossTradeMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::BrokenTradeMessage(const ITCH::MessageHeader& header) {
    ITCH::BrokenTradeMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::NOIIMessage(const ITCH::MessageHeader& header) {
    ITCH::NOIIMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::DLCRMessage(const ITCH::MessageHeader& header) {
    ITCH::DLCRMessage msg = {};
    m_MessageQueue.push(msg);
}

auto ITCH_Parser::RetailPriceImprovementIndicatorMessage(const ITCH::MessageHeader& header) {
    ITCH::RetailPriceImprovementIndicatorMessage msg = {};
    m_MessageQueue.push(msg);
}

void ITCH_Parser::Execute() {
    while (m_Reader.tell() < m_Reader.size()) {
        // The first two bytes are message length as per moldupd64
        // can skip, jumping to the message type char
        m_Reader.seek(m_Reader.tell() + 2);

        // call each message as par the message type
        ITCH::Message decoded_message =
            m_dispatch[static_cast<ITCH::MessageType>(m_Reader.ReadChar())]();
        m_MessageQueue.push(decoded_message);
    }
}

ITCH::Message ITCH_Parser::DecodeMessage(ITCH::MessageType message_id) {
    // skip the message type byte. Type is alReady know
    m_Reader.seek(m_Reader.tell() + 1);

    return m_dispatch[message_id]();
}

// bool ITCH_Parser::Execute() {
//     bool success_flag = true;
//     while (m_Reader.tell() < m_Reader.size() && success_flag) {
//         // The first two bytes are message length as per moldupd64
//         // can skip, jumping to the message type char
//         m_Reader.seek(m_Reader.tell() + 2);

//         ITCH::MessageHeader header = {.message_type = (ITCH::MessageType)m_Reader.ReadChar(),
//                                       .stock_locate = m_Reader.Read16(),
//                                       .tracking_number = m_Reader.Read16(),
//                                       .timestamp = m_Reader.Read48()};

//         switch (header.message_type) {
//             case ITCH::MessageType::SystemEventMessage: {
//                 SystemEventMessage();
//                 break;
//             }
//             case ITCH::MessageType::StockDirectoryMessage: {
//                 StockDirectoryMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::StockTradingActionMessage: {
//                 StockTradingActionMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::RegSHOMessage: {
//                 RegSHOMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::MarketParticipantPositionMessage: {
//                 MarketParticipantPositionMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::MWCBDeclineLevelMessage: {
//                 MWCBDeclineLevelMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::MWCBStatusMessage: {
//                 MWCBStatusMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::IPOQuotingPeriodUpdateMessage: {
//                 IPOQuotingPeriodUpdateMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::LULDAuctionCollarMessage: {
//                 LULDAuctionCollarMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::OperationalHaltMessage: {
//                 OperationalHaltMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::AddOrderMessage: {
//                 AddOrderMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::AddOrderMPIDAttributionMessage: {
//                 AddOrderMPIDAttributionMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::OrderExecutedMessage: {
//                 OrderExecutedMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::OrderExecutedWithPriceMessage: {
//                 OrderExecutedWithPriceMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::OrderCancelMessage: {
//                 OrderCancelMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::OrderDeleteMessage: {
//                 OrderDeleteMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::OrderReplaceMessage: {
//                 OrderReplaceMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::NonCrossTradeMessage: {
//                 NonCrossTradeMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::CrossTradeMessage: {
//                 CrossTradeMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::BrokenTradeMessage: {
//                 BrokenTradeMessage(header);
//                 break;
//             }
//             case ITCH::MessageType::RetailPriceImprovementIndicatorMessage: {
//                 RetailPriceImprovementIndicatorMessage(header);
//                 break;
//             }
//             default:
//                 success_flag = false;
//         }
//     }
//     return success_flag;
// }
