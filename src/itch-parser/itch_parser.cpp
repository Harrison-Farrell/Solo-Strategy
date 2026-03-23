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
auto ITCH_Parser::SystemEventMessage() { return m_File.Read<ITCH::SystemEventMessage>(); }
auto ITCH_Parser::StockTradingActionMessage() { return m_File.Read<ITCH::StockTradingActionMessage>(); }
auto ITCH_Parser::RegSHOMessage() { return m_File.Read<ITCH::RegSHOMessage>(); }
auto ITCH_Parser::MarketParticipantPositionMessage() { return m_File.Read<ITCH::MarketParticipantPositionMessage>(); }
auto ITCH_Parser::MWCBDeclineLevelMessage() { return m_File.Read<ITCH::MWCBDeclineLevelMessage>(); }
auto ITCH_Parser::MWCBStatusMessage() { return m_File.Read<ITCH::MWCBStatusMessage>(); }
auto ITCH_Parser::StockDirectoryMessage() { return m_File.Read<ITCH::StockDirectoryMessage>(); }
auto ITCH_Parser::IPOQuotingPeriodUpdateMessage() { return m_File.Read<ITCH::IPOQuotingPeriodUpdateMessage>(); }
auto ITCH_Parser::LULDAuctionCollarMessage() { return m_File.Read<ITCH::LULDAuctionCollarMessage>(); }
auto ITCH_Parser::OperationalHaltMessage() { return m_File.Read<ITCH::OperationalHaltMessage>(); }
auto ITCH_Parser::AddOrderMessage() { return m_File.Read<ITCH::AddOrderMessage>(); }
auto ITCH_Parser::AddOrderMPIDAttributionMessage() { return m_File.Read<ITCH::AddOrderMPIDAttributionMessage>(); }
auto ITCH_Parser::OrderExecutedMessage() { return m_File.Read<ITCH::OrderExecutedMessage>(); }
auto ITCH_Parser::OrderExecutedWithPriceMessage() { return m_File.Read<ITCH::OrderExecutedWithPriceMessage>(); }
auto ITCH_Parser::OrderCancelMessage() { return m_File.Read<ITCH::OrderCancelMessage>(); }
auto ITCH_Parser::OrderDeleteMessage() { return m_File.Read<ITCH::OrderDeleteMessage>(); }
// clang-format on

ITCH_Parser::ITCH_Parser(const std::string& file_path)
    : m_File(file_path), m_MessageQueue(m_MessageQueueSize) {
    using namespace ITCH;
    // clang-format off
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
    m_dispatch[MessageType::AddOrderMessage] = [this](){return AddOrderMessage();};
    m_dispatch[MessageType::AddOrderMPIDAttributionMessage] = [this](){return AddOrderMPIDAttributionMessage();};
    m_dispatch[MessageType::OrderExecutedMessage] = [this](){return OrderExecutedMessage();};
    m_dispatch[MessageType::OrderExecutedWithPriceMessage] = [this](){return OrderExecutedWithPriceMessage();};
    m_dispatch[MessageType::OrderDeleteMessage] = [this](){return OrderDeleteMessage();};
    m_dispatch[MessageType::OrderCancelMessage] = [this](){return OrderCancelMessage();};
    // clang-format on
}

auto ITCH_Parser::OrderReplaceMessage(const ITCH::MessageHeader& header) {
    ITCH::OrderReplaceMessage msg = {};
    m_MessageQueue.Push(msg);
}

auto ITCH_Parser::NonCrossTradeMessage(const ITCH::MessageHeader& header) {
    ITCH::NonCrossTradeMessage msg = {};
    m_MessageQueue.Push(msg);
}

auto ITCH_Parser::CrossTradeMessage(const ITCH::MessageHeader& header) {
    ITCH::CrossTradeMessage msg = {};
    m_MessageQueue.Push(msg);
}

auto ITCH_Parser::BrokenTradeMessage(const ITCH::MessageHeader& header) {
    ITCH::BrokenTradeMessage msg = {};
    m_MessageQueue.Push(msg);
}

auto ITCH_Parser::NOIIMessage(const ITCH::MessageHeader& header) {
    ITCH::NOIIMessage msg = {};
    m_MessageQueue.Push(msg);
}

auto ITCH_Parser::DLCRMessage(const ITCH::MessageHeader& header) {
    ITCH::DLCRMessage msg = {};
    m_MessageQueue.Push(msg);
}

auto ITCH_Parser::RetailPriceImprovementIndicatorMessage(const ITCH::MessageHeader& header) {
    ITCH::RetailPriceImprovementIndicatorMessage msg = {};
    m_MessageQueue.Push(msg);
}

void ITCH_Parser::Execute() {
    while (m_File.tell() < m_File.size()) {
        // The first two bytes are message length as per moldupd64
        // Skip, jumping to the message type char
        m_File.seek(m_File.tell() + 2);
        m_MessageQueue.Push(DecodeMessage());
    }
}

ITCH::Message ITCH_Parser::DecodeMessage() {
    const auto type = m_File.Inspect<ITCH::MessageType>();
    return m_dispatch[type]();
}
