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
#include "thread/thread.h"

ITCH_Parser::ITCH_Parser(
    std::shared_ptr<LockFreeQueue<RawItchPacket>>& input_queue,
    std::shared_ptr<LockFreeQueue<ITCH::Message>>& output_queue)
    : m_InputQueue(input_queue), m_OutputQueue(output_queue) {}

auto ITCH_Parser::SystemEventMessage(const uint8_t* buffer)
    -> ITCH::SystemEventMessage {
    return *reinterpret_cast<const ITCH::SystemEventMessage*>(buffer);
}

auto ITCH_Parser::StockTradingActionMessage(const uint8_t* buffer)
    -> ITCH::StockTradingActionMessage {
    return *reinterpret_cast<const ITCH::StockTradingActionMessage*>(buffer);
}

auto ITCH_Parser::RegSHOMessage(const uint8_t* buffer) -> ITCH::RegSHOMessage {
    return *reinterpret_cast<const ITCH::RegSHOMessage*>(buffer);
}

auto ITCH_Parser::MarketParticipantPositionMessage(const uint8_t* buffer)
    -> ITCH::MarketParticipantPositionMessage {
    return *reinterpret_cast<const ITCH::MarketParticipantPositionMessage*>(
        buffer);
}

auto ITCH_Parser::MWCBDeclineLevelMessage(const uint8_t* buffer)
    -> ITCH::MWCBDeclineLevelMessage {
    return *reinterpret_cast<const ITCH::MWCBDeclineLevelMessage*>(buffer);
}

auto ITCH_Parser::MWCBStatusMessage(const uint8_t* buffer)
    -> ITCH::MWCBStatusMessage {
    return *reinterpret_cast<const ITCH::MWCBStatusMessage*>(buffer);
}

auto ITCH_Parser::StockDirectoryMessage(const uint8_t* buffer)
    -> ITCH::StockDirectoryMessage {
    return *reinterpret_cast<const ITCH::StockDirectoryMessage*>(buffer);
}

auto ITCH_Parser::IPOQuotingPeriodUpdateMessage(const uint8_t* buffer)
    -> ITCH::IPOQuotingPeriodUpdateMessage {
    return *reinterpret_cast<const ITCH::IPOQuotingPeriodUpdateMessage*>(
        buffer);
}

auto ITCH_Parser::LULDAuctionCollarMessage(const uint8_t* buffer)
    -> ITCH::LULDAuctionCollarMessage {
    return *reinterpret_cast<const ITCH::LULDAuctionCollarMessage*>(buffer);
}

auto ITCH_Parser::OperationalHaltMessage(const uint8_t* buffer)
    -> ITCH::OperationalHaltMessage {
    return *reinterpret_cast<const ITCH::OperationalHaltMessage*>(buffer);
}

auto ITCH_Parser::AddOrderMessage(const uint8_t* buffer)
    -> ITCH::AddOrderMessage {
    return *reinterpret_cast<const ITCH::AddOrderMessage*>(buffer);
}

auto ITCH_Parser::AddOrderMPIDAttributionMessage(const uint8_t* buffer)
    -> ITCH::AddOrderMPIDAttributionMessage {
    return *reinterpret_cast<const ITCH::AddOrderMPIDAttributionMessage*>(
        buffer);
}

auto ITCH_Parser::OrderExecutedMessage(const uint8_t* buffer)
    -> ITCH::OrderExecutedMessage {
    return *reinterpret_cast<const ITCH::OrderExecutedMessage*>(buffer);
}

auto ITCH_Parser::OrderExecutedWithPriceMessage(const uint8_t* buffer)
    -> ITCH::OrderExecutedWithPriceMessage {
    return *reinterpret_cast<const ITCH::OrderExecutedWithPriceMessage*>(
        buffer);
}

auto ITCH_Parser::OrderCancelMessage(const uint8_t* buffer)
    -> ITCH::OrderCancelMessage {
    return *reinterpret_cast<const ITCH::OrderCancelMessage*>(buffer);
}

auto ITCH_Parser::OrderDeleteMessage(const uint8_t* buffer)
    -> ITCH::OrderDeleteMessage {
    return *reinterpret_cast<const ITCH::OrderDeleteMessage*>(buffer);
}

auto ITCH_Parser::OrderReplaceMessage(const uint8_t* buffer)
    -> ITCH::OrderReplaceMessage {
    return *reinterpret_cast<const ITCH::OrderReplaceMessage*>(buffer);
}

auto ITCH_Parser::NonCrossTradeMessage(const uint8_t* buffer)
    -> ITCH::NonCrossTradeMessage {
    return *reinterpret_cast<const ITCH::NonCrossTradeMessage*>(buffer);
}

auto ITCH_Parser::CrossTradeMessage(const uint8_t* buffer)
    -> ITCH::CrossTradeMessage {
    return *reinterpret_cast<const ITCH::CrossTradeMessage*>(buffer);
}

auto ITCH_Parser::BrokenTradeMessage(const uint8_t* buffer)
    -> ITCH::BrokenTradeMessage {
    return *reinterpret_cast<const ITCH::BrokenTradeMessage*>(buffer);
}

auto ITCH_Parser::NOIIMessage(const uint8_t* buffer) -> ITCH::NOIIMessage {
    return *reinterpret_cast<const ITCH::NOIIMessage*>(buffer);
}

auto ITCH_Parser::DLCRMessage(const uint8_t* buffer) -> ITCH::DLCRMessage {
    return *reinterpret_cast<const ITCH::DLCRMessage*>(buffer);
}

auto ITCH_Parser::RetailPriceImprovementIndicatorMessage(const uint8_t* buffer)
    -> ITCH::RetailPriceImprovementIndicatorMessage {
    return *reinterpret_cast<
        const ITCH::RetailPriceImprovementIndicatorMessage*>(buffer);
}

auto ITCH_Parser::Execute() -> void {
    while (true) {
        const auto* packet = m_InputQueue->GetNextRead();
        if (packet) {
            // A length of 0 is our sentinel for termination
            if (packet->length == 0) [[unlikely]] {
                m_InputQueue->UpdateReadIndex();
                while (!m_OutputQueue->Push(ITCH::Message{})) {
                    std::this_thread::yield();
                }
                break;
            }

            // Decode the message and push it to the output queue
            m_OutputQueue->Push(DecodeMessage(packet->data.data()));
            m_InputQueue->UpdateReadIndex();
        } else {
            // Yield to avoid busy spinning at 100% CPU when starved
            std::this_thread::yield();
        }
    }
}

std::shared_ptr<std::thread> ITCH_Parser::Start(int core_id) {
    return CreateAndStartThread(core_id, "ITCH Parser",
                                [this]() { Execute(); });
}

ITCH::Message ITCH_Parser::DecodeMessage(const uint8_t* buffer) {
    const auto type = *reinterpret_cast<const ITCH::MessageType*>(buffer);
    switch (type) {
        case ITCH::MessageType::SystemEventMessage:
            return SystemEventMessage(buffer);
        case ITCH::MessageType::StockDirectoryMessage:
            return StockDirectoryMessage(buffer);
        case ITCH::MessageType::StockTradingActionMessage:
            return StockTradingActionMessage(buffer);
        case ITCH::MessageType::RegSHOMessage:
            return RegSHOMessage(buffer);
        case ITCH::MessageType::MarketParticipantPositionMessage:
            return MarketParticipantPositionMessage(buffer);
        case ITCH::MessageType::MWCBDeclineLevelMessage:
            return MWCBDeclineLevelMessage(buffer);
        case ITCH::MessageType::MWCBStatusMessage:
            return MWCBStatusMessage(buffer);
        case ITCH::MessageType::IPOQuotingPeriodUpdateMessage:
            return IPOQuotingPeriodUpdateMessage(buffer);
        case ITCH::MessageType::LULDAuctionCollarMessage:
            return LULDAuctionCollarMessage(buffer);
        case ITCH::MessageType::OperationalHaltMessage:
            return OperationalHaltMessage(buffer);
        case ITCH::MessageType::AddOrderMessage:
            return AddOrderMessage(buffer);
        case ITCH::MessageType::AddOrderMPIDAttributionMessage:
            return AddOrderMPIDAttributionMessage(buffer);
        case ITCH::MessageType::OrderExecutedMessage:
            return OrderExecutedMessage(buffer);
        case ITCH::MessageType::OrderExecutedWithPriceMessage:
            return OrderExecutedWithPriceMessage(buffer);
        case ITCH::MessageType::OrderCancelMessage:
            return OrderCancelMessage(buffer);
        case ITCH::MessageType::OrderDeleteMessage:
            return OrderDeleteMessage(buffer);
        case ITCH::MessageType::OrderReplaceMessage:
            return OrderReplaceMessage(buffer);
        case ITCH::MessageType::NonCrossTradeMessage:
            return NonCrossTradeMessage(buffer);
        case ITCH::MessageType::CrossTradeMessage:
            return CrossTradeMessage(buffer);
        case ITCH::MessageType::BrokenTradeMessage:
            return BrokenTradeMessage(buffer);
        case ITCH::MessageType::NOIIMessage:
            return NOIIMessage(buffer);
        case ITCH::MessageType::DLCRMessage:
            return DLCRMessage(buffer);
        case ITCH::MessageType::RetailPriceImprovementIndicatorMessage:
            return RetailPriceImprovementIndicatorMessage(buffer);
        default:
            return ITCH::Message{};
    }
}
