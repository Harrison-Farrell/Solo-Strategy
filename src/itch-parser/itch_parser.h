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

// Type definition for the function returns void
using DispatchFunction = std::function<ITCH::Message()>;

class ITCH_Parser {
   public:
    ITCH_Parser(const std::string& file_path);

    ITCH::Message DecodeMessage();
    void Execute();

   private:
    MemoryMappedFile m_Reader;
    const int m_MessageQueueSize = 1024;
    LockFreeQueue<ITCH::Message> m_MessageQueue;
    std::unordered_map<ITCH::MessageType, DispatchFunction> m_dispatch;

    // unit tested messages
    inline auto SystemEventMessage();
    inline auto StockTradingActionMessage();
    inline auto RegSHOMessage();
    inline auto MarketParticipantPositionMessage();
    inline auto MWCBDeclineLevelMessage();
    inline auto MWCBStatusMessage();
    // to be unit tested
    inline auto StockDirectoryMessage(const ITCH::MessageHeader& header);

    inline auto IPOQuotingPeriodUpdateMessage(const ITCH::MessageHeader& header);
    inline auto LULDAuctionCollarMessage(const ITCH::MessageHeader& header);
    inline auto OperationalHaltMessage(const ITCH::MessageHeader& header);
    inline auto AddOrderMessage(const ITCH::MessageHeader& header);
    inline auto AddOrderMPIDAttributionMessage(const ITCH::MessageHeader& header);
    inline auto OrderExecutedMessage(const ITCH::MessageHeader& header);
    inline auto OrderExecutedWithPriceMessage(const ITCH::MessageHeader& header);
    inline auto OrderCancelMessage(const ITCH::MessageHeader& header);
    inline auto OrderDeleteMessage(const ITCH::MessageHeader& header);
    inline auto OrderReplaceMessage(const ITCH::MessageHeader& header);
    inline auto NonCrossTradeMessage(const ITCH::MessageHeader& header);
    inline auto CrossTradeMessage(const ITCH::MessageHeader& header);
    inline auto BrokenTradeMessage(const ITCH::MessageHeader& header);
    inline auto NOIIMessage(const ITCH::MessageHeader& header);
    inline auto DLCRMessage(const ITCH::MessageHeader& header);
    inline auto RetailPriceImprovementIndicatorMessage(const ITCH::MessageHeader& header);
};

#endif  // SOLO_STRATEGY_SRC_ITCH_PARSER_ITCH_PARSER_H_