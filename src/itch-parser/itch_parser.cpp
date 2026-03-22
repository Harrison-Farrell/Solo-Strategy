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

// clang-format off
auto ITCH_Parser::SystemEventMessage() { return m_Reader.Read<ITCH::SystemEventMessage>(); }

auto ITCH_Parser::StockTradingActionMessage() { return m_Reader.Read<ITCH::StockTradingActionMessage>(); }

auto ITCH_Parser::RegSHOMessage() { return m_Reader.Read<ITCH::RegSHOMessage>(); }

auto ITCH_Parser::MarketParticipantPositionMessage() { return m_Reader.Read<ITCH::MarketParticipantPositionMessage>(); }

auto ITCH_Parser::MWCBDeclineLevelMessage() { return m_Reader.Read<ITCH::MWCBDeclineLevelMessage>(); }

auto ITCH_Parser::MWCBStatusMessage() { return m_Reader.Read<ITCH::MWCBStatusMessage>(); }

auto ITCH_Parser::StockDirectoryMessage() { return m_Reader.Read<ITCH::StockDirectoryMessage>(); }

auto ITCH_Parser::IPOQuotingPeriodUpdateMessage() { return m_Reader.Read<ITCH::IPOQuotingPeriodUpdateMessage>(); }

auto ITCH_Parser::LULDAuctionCollarMessage() { return m_Reader.Read<ITCH::LULDAuctionCollarMessage>(); }

auto ITCH_Parser::OperationalHaltMessage() { return m_Reader.Read<ITCH::OperationalHaltMessage>(); }
// clang-format on

ITCH_Parser::ITCH_Parser(const std::string& file_path)
    : m_Reader(file_path), m_MessageQueue(m_MessageQueueSize) {
    // clang-format off
    using namespace ITCH;
    m_dispatch[MessageType::SystemEventMessage] = [this](){return SystemEventMessage();};
    m_dispatch[MessageType::StockTradingActionMessage] = [this](){return StockTradingActionMessage();};
    m_dispatch[MessageType::RegSHOMessage] = [this](){return RegSHOMessage();};
    m_dispatch[MessageType::MarketParticipantPositionMessage] = [this](){return MarketParticipantPositionMessage();};
    m_dispatch[MessageType::MWCBDeclineLevelMessage] = [this](){return MWCBDeclineLevelMessage();};
    m_dispatch[MessageType::MWCBStatusMessage] = [this](){return MWCBStatusMessage();};
    m_dispatch[MessageType::StockDirectoryMessage] = [this](){return StockDirectoryMessage();};
    m_dispatch[MessageType::IPOQuotingPeriodUpdateMessage] = [this](){return IPOQuotingPeriodUpdateMessage();};
    m_dispatch[MessageType::LULDAuctionCollarMessage] = [this](){return LULDAuctionCollarMessage();};
    m_dispatch[MessageType::OperationalHaltMessage] = [this](){return OperationalHaltMessage();};
    // clang-format on
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
        // Skip, jumping to the message type char
        m_Reader.seek(m_Reader.tell() + 2);
        m_MessageQueue.push(DecodeMessage());
    }
}

ITCH::Message ITCH_Parser::DecodeMessage() {
    const auto type = m_Reader.Inspect<ITCH::MessageType>();
    return m_dispatch[type]();
}
