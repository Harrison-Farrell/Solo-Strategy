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

ITCH_Parser::ITCH_Parser(const std::string& file_path)
    : m_File(file_path), m_MessageQueue(m_MessageQueueSize) {}

auto ITCH_Parser::SystemEventMessage() -> ITCH::Message {
    return m_File.Read<ITCH::SystemEventMessage>();
}

auto ITCH_Parser::StockTradingActionMessage() -> ITCH::Message {
    return m_File.Read<ITCH::StockTradingActionMessage>();
}

auto ITCH_Parser::RegSHOMessage() -> ITCH::Message {
    return m_File.Read<ITCH::RegSHOMessage>();
}

auto ITCH_Parser::MarketParticipantPositionMessage() -> ITCH::Message {
    return m_File.Read<ITCH::MarketParticipantPositionMessage>();
}
auto ITCH_Parser::MWCBDeclineLevelMessage() -> ITCH::Message {
    return m_File.Read<ITCH::MWCBDeclineLevelMessage>();
}

auto ITCH_Parser::MWCBStatusMessage() -> ITCH::Message {
    return m_File.Read<ITCH::MWCBStatusMessage>();
}

auto ITCH_Parser::StockDirectoryMessage() -> ITCH::Message {
    return m_File.Read<ITCH::StockDirectoryMessage>();
}

auto ITCH_Parser::IPOQuotingPeriodUpdateMessage() -> ITCH::Message {
    return m_File.Read<ITCH::IPOQuotingPeriodUpdateMessage>();
}

auto ITCH_Parser::LULDAuctionCollarMessage() -> ITCH::Message {
    return m_File.Read<ITCH::LULDAuctionCollarMessage>();
}

auto ITCH_Parser::OperationalHaltMessage() -> ITCH::Message {
    return m_File.Read<ITCH::OperationalHaltMessage>();
}

auto ITCH_Parser::AddOrderMessage() -> ITCH::Message {
    return m_File.Read<ITCH::AddOrderMessage>();
}

auto ITCH_Parser::AddOrderMPIDAttributionMessage() -> ITCH::Message {
    return m_File.Read<ITCH::AddOrderMPIDAttributionMessage>();
}

auto ITCH_Parser::OrderExecutedMessage() -> ITCH::Message {
    return m_File.Read<ITCH::OrderExecutedMessage>();
}

auto ITCH_Parser::OrderExecutedWithPriceMessage() -> ITCH::Message {
    return m_File.Read<ITCH::OrderExecutedWithPriceMessage>();
}

auto ITCH_Parser::OrderCancelMessage() -> ITCH::Message {
    return m_File.Read<ITCH::OrderCancelMessage>();
}

auto ITCH_Parser::OrderDeleteMessage() -> ITCH::Message {
    return m_File.Read<ITCH::OrderDeleteMessage>();
}

auto ITCH_Parser::OrderReplaceMessage() -> ITCH::Message {
    return m_File.Read<ITCH::OrderReplaceMessage>();
}

auto ITCH_Parser::NonCrossTradeMessage() -> ITCH::Message {
    return m_File.Read<ITCH::NonCrossTradeMessage>();
}

auto ITCH_Parser::CrossTradeMessage() -> ITCH::Message {
    return m_File.Read<ITCH::CrossTradeMessage>();
}

auto ITCH_Parser::BrokenTradeMessage() -> ITCH::Message {
    return m_File.Read<ITCH::BrokenTradeMessage>();
}

auto ITCH_Parser::NOIIMessage() -> ITCH::Message {
    return m_File.Read<ITCH::NOIIMessage>();
}

auto ITCH_Parser::DLCRMessage(const ITCH::MessageHeader& header) {
    ITCH::DLCRMessage msg = {};
    m_MessageQueue.Push(msg);
}

auto ITCH_Parser::RetailPriceImprovementIndicatorMessage(
    const ITCH::MessageHeader& header) {
    ITCH::RetailPriceImprovementIndicatorMessage msg = {};
    m_MessageQueue.Push(msg);
}

void ITCH_Parser::Execute() {
    while (m_File.tell() < m_File.size()) {
        // The first two bytes are message length as per moldupd64
        // skip to the message type char
        m_File.seek(m_File.tell() + 2);
        m_MessageQueue.Push(DecodeMessage());
    }
}

ITCH::Message ITCH_Parser::DecodeMessage() {
    const auto type = m_File.Inspect<ITCH::MessageType>();
    switch (type) {
        case ITCH::MessageType::SystemEventMessage:
            return SystemEventMessage();
            break;
        case ITCH::MessageType::StockDirectoryMessage:
            return StockDirectoryMessage();
            break;
        case ITCH::MessageType::StockTradingActionMessage:
            return StockTradingActionMessage();
            break;
        case ITCH::MessageType::RegSHOMessage:
            return RegSHOMessage();
            break;
        case ITCH::MessageType::MarketParticipantPositionMessage:
            return MarketParticipantPositionMessage();
            break;
        case ITCH::MessageType::MWCBDeclineLevelMessage:
            return MWCBDeclineLevelMessage();
            break;
        case ITCH::MessageType::MWCBStatusMessage:
            return MWCBStatusMessage();
            break;
        case ITCH::MessageType::IPOQuotingPeriodUpdateMessage:
            return IPOQuotingPeriodUpdateMessage();
            break;
        case ITCH::MessageType::LULDAuctionCollarMessage:
            return LULDAuctionCollarMessage();
            break;
        case ITCH::MessageType::OperationalHaltMessage:
            return OperationalHaltMessage();
            break;
        case ITCH::MessageType::AddOrderMessage:
            return AddOrderMessage();
            break;
        case ITCH::MessageType::AddOrderMPIDAttributionMessage:
            return AddOrderMPIDAttributionMessage();
            break;
        case ITCH::MessageType::OrderExecutedMessage:
            return OrderExecutedMessage();
            break;
        case ITCH::MessageType::OrderExecutedWithPriceMessage:
            return OrderExecutedWithPriceMessage();
            break;
        case ITCH::MessageType::OrderCancelMessage:
            return OrderCancelMessage();
            break;
        case ITCH::MessageType::OrderDeleteMessage:
            return OrderDeleteMessage();
            break;
        case ITCH::MessageType::OrderReplaceMessage:
            return OrderReplaceMessage();
            break;
        case ITCH::MessageType::NonCrossTradeMessage:
            return NonCrossTradeMessage();
            break;
        case ITCH::MessageType::CrossTradeMessage:
            return CrossTradeMessage();
            break;
        case ITCH::MessageType::BrokenTradeMessage:
            return BrokenTradeMessage();
            break;
        case ITCH::MessageType::NOIIMessage:
            return NOIIMessage();
            break;
        // case ITCH::MessageType::DLCRMessage:
        //     return DLCRMessage();
        //     break;
        // case ITCH::MessageType::RetailPriceImprovementIndicatorMessage:
        //     return RetailPriceImprovementIndicatorMessage();
        //     break;
        default:
            return ITCH::Message{};
    }
}
