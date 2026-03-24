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

#ifndef SOLO_STRATEGY_SRC_ITCH_PARSER_ITCH_PARSER_H_
#define SOLO_STRATEGY_SRC_ITCH_PARSER_ITCH_PARSER_H_

#include <functional>
#include <string>
#include <unordered_map>

#include "itch-parser/messages/itch_messages.h"
#include "lock-free-queue/lock_free_queue.h"
#include "memory-map/memory_map_file.h"

class ITCH_Parser {
   public:
    ITCH_Parser(const std::string& file_path);

    void Execute();
    ITCH::Message DecodeMessage();

   private:
    MemoryMappedFile m_File;
    const int m_MessageQueueSize = 1024;
    LockFreeQueue<ITCH::Message> m_MessageQueue;
    std::unordered_map<ITCH::MessageType, std::function<ITCH::Message()>>
        m_dispatch;

    /// \brief The system event message type is used to signal a market or data
    /// feed handler event
    /// \return ITCH::Message Variant<SystemEventMesaage>
    inline auto SystemEventMessage() -> ITCH::Message;

    /// \brief  At the start of each trading day, Nasdaq disseminates stock
    /// directory messages for all active symbols in the Nasdaq execution
    /// system.
    /// \return ITCH::Message Variant<StockDirectoryMessage>
    inline auto StockDirectoryMessage() -> ITCH::Message;

    /// \brief Nasdaq uses this administrative message to indicate the current
    /// trading status of a security to the trading community
    /// \return ITCH::Message Variant<StockTradingActionMessage>
    inline auto StockTradingActionMessage() -> ITCH::Message;

    /// \brief  The Reg SHO Short Sale Price Test Restricted Indicator signifies
    /// that Rule 201 has been triggered for a specific security because it
    /// dropped 10% or more from the previous day's close
    /// \return ITCH::Message Variant<RegSHOMessage>
    inline auto RegSHOMessage() -> ITCH::Message;

    /// \brief  At the start of each trading day, Nasdaq disseminates a spin of
    /// market participant position messages. The message provides the Primary
    /// Market Maker status, Market Maker mode and Market Participant state for
    /// each Nasdaq market participant firm registered in an issue.
    /// \return ITCH::Message Variant<MarketParticipantPositionMessage>
    inline auto MarketParticipantPositionMessage() -> ITCH::Message;

    /// \brief Informs data recipients what the daily MWCB breach points are set
    /// to for the current trading day.
    /// \return ITCH::Message Variant<MWCBDeclineLevelMessage>
    inline auto MWCBDeclineLevelMessage() -> ITCH::Message;

    /// \brief Informs data recipients when a MWCB has breached one of the
    /// established levels
    /// \return ITCH::Message Variant<MWCBStatusMessage>
    inline auto MWCBStatusMessage() -> ITCH::Message;

    /// \brief Indicates the anticipated IPO quotation release time of a
    /// security.
    /// \return ITCH::Message Variant<IPOQuotingPeriodUpdateMessage>
    inline auto IPOQuotingPeriodUpdateMessage() -> ITCH::Message;

    /// \brief Indicates the auction collar thresholds within which a paused
    /// security can reopen following a LULD Trading Pause.
    /// \return ITCH::Message Variant<LULDAuctionCollarMessage>
    inline auto LULDAuctionCollarMessage() -> ITCH::Message;

    /// \brief The Exchange uses this message to indicate the current
    /// Operational Status of a security to the trading community.
    /// \return ITCH::Message Variant<OperationalHaltMessage>
    inline auto OperationalHaltMessage() -> ITCH::Message;
    inline auto AddOrderMessage() -> ITCH::Message;
    inline auto AddOrderMPIDAttributionMessage() -> ITCH::Message;
    inline auto OrderExecutedMessage() -> ITCH::Message;
    inline auto OrderExecutedWithPriceMessage() -> ITCH::Message;
    inline auto OrderCancelMessage() -> ITCH::Message;
    inline auto OrderDeleteMessage() -> ITCH::Message;
    inline auto OrderReplaceMessage() -> ITCH::Message;
    // to be unit tested

    inline auto NonCrossTradeMessage(const ITCH::MessageHeader& header);
    inline auto CrossTradeMessage(const ITCH::MessageHeader& header);
    inline auto BrokenTradeMessage(const ITCH::MessageHeader& header);
    inline auto NOIIMessage(const ITCH::MessageHeader& header);
    inline auto DLCRMessage(const ITCH::MessageHeader& header);
    inline auto RetailPriceImprovementIndicatorMessage(
        const ITCH::MessageHeader& header);
};

#endif  // SOLO_STRATEGY_SRC_ITCH_PARSER_ITCH_PARSER_H_