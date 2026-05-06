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

// system includes
#include <memory>
#include <string>
#include <thread>

// local includes
#include "itch-parser/messages/itch_messages.h"
#include "lock-free-queue/lock_free_queue.h"
#include "memory-map/memory_map_file.h"
#include "thread/thread.h"

ITCH_Parser::ITCH_Parser(
    const std::string& file_path,
    std::shared_ptr<LockFreeQueue<ITCH::Message>>& queue_pointer)
    : m_File(file_path), m_MessageQueue(queue_pointer) {}

auto ITCH_Parser::SystemEventMessage() {
    return m_File.Read<ITCH::SystemEventMessage>();
}

auto ITCH_Parser::StockTradingActionMessage() {
    return m_File.Read<ITCH::StockTradingActionMessage>();
}

auto ITCH_Parser::RegSHOMessage() { return m_File.Read<ITCH::RegSHOMessage>(); }

auto ITCH_Parser::MarketParticipantPositionMessage() {
    return m_File.Read<ITCH::MarketParticipantPositionMessage>();
}
auto ITCH_Parser::MWCBDeclineLevelMessage() {
    return m_File.Read<ITCH::MWCBDeclineLevelMessage>();
}

auto ITCH_Parser::MWCBStatusMessage() {
    return m_File.Read<ITCH::MWCBStatusMessage>();
}

auto ITCH_Parser::StockDirectoryMessage() {
    return m_File.Read<ITCH::StockDirectoryMessage>();
}

auto ITCH_Parser::IPOQuotingPeriodUpdateMessage() {
    return m_File.Read<ITCH::IPOQuotingPeriodUpdateMessage>();
}

auto ITCH_Parser::LULDAuctionCollarMessage() {
    return m_File.Read<ITCH::LULDAuctionCollarMessage>();
}

auto ITCH_Parser::OperationalHaltMessage() {
    return m_File.Read<ITCH::OperationalHaltMessage>();
}

auto ITCH_Parser::AddOrderMessage() {
    return m_File.Read<ITCH::AddOrderMessage>();
}

auto ITCH_Parser::AddOrderMPIDAttributionMessage() {
    return m_File.Read<ITCH::AddOrderMPIDAttributionMessage>();
}

auto ITCH_Parser::OrderExecutedMessage() {
    return m_File.Read<ITCH::OrderExecutedMessage>();
}

auto ITCH_Parser::OrderExecutedWithPriceMessage() {
    return m_File.Read<ITCH::OrderExecutedWithPriceMessage>();
}

auto ITCH_Parser::OrderCancelMessage() {
    return m_File.Read<ITCH::OrderCancelMessage>();
}

auto ITCH_Parser::OrderDeleteMessage() {
    return m_File.Read<ITCH::OrderDeleteMessage>();
}

auto ITCH_Parser::OrderReplaceMessage() {
    return m_File.Read<ITCH::OrderReplaceMessage>();
}

auto ITCH_Parser::NonCrossTradeMessage() {
    return m_File.Read<ITCH::NonCrossTradeMessage>();
}

auto ITCH_Parser::CrossTradeMessage() {
    return m_File.Read<ITCH::CrossTradeMessage>();
}

auto ITCH_Parser::BrokenTradeMessage() {
    return m_File.Read<ITCH::BrokenTradeMessage>();
}

auto ITCH_Parser::NOIIMessage() { return m_File.Read<ITCH::NOIIMessage>(); }

auto ITCH_Parser::DLCRMessage() { return m_File.Read<ITCH::DLCRMessage>(); }

auto ITCH_Parser::RetailPriceImprovementIndicatorMessage() {
    return m_File.Read<ITCH::RetailPriceImprovementIndicatorMessage>();
}

auto ITCH_Parser::Execute() {
    while (m_File.Tell() < m_File.Size()) {
        // The first two bytes are message length as per moldupd64
        // skip to the message type char
        m_File.Seek(m_File.Tell() + 2);
        m_MessageQueue->Push(DecodeMessage());
    }
}

std::shared_ptr<std::thread> ITCH_Parser::Start(int core_id) {
    return CreateAndStartThread(core_id, "ITCH Parser",
                                [this]() { Execute(); });
}

ITCH::Message ITCH_Parser::DecodeMessage() {
    const auto& type = m_File.Inspect<ITCH::MessageType>();
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
        case ITCH::MessageType::DLCRMessage:
            return DLCRMessage();
            break;
        case ITCH::MessageType::RetailPriceImprovementIndicatorMessage:
            return RetailPriceImprovementIndicatorMessage();
            break;
        default:
            return ITCH::Message{};
    }
}
