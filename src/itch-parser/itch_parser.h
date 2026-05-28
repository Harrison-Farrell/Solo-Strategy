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

// system includes
#include <atomic>
#include <memory>
#include <thread>

// local includes
#include "itch-parser/messages/itch_messages.h"
#include "lock-free-queue/lock_free_queue.h"

namespace {
constexpr uint32_t MAXIMUM_PACKET_SIZE{64};
}

/// \brief A fixed-size packet to safely pass raw ITCH messages across lock-free
/// queues
struct RawItchPacket {
    std::array<uint8_t, MAXIMUM_PACKET_SIZE> data;
    uint16_t length;
};

class ITCH_Parser {
   public:
    ITCH_Parser(std::shared_ptr<LockFreeQueue<RawItchPacket>>& input_queue,
                std::shared_ptr<LockFreeQueue<ITCH::Message>>& output_queue);

    /// \brief Executes the Execute() function on the newly created thread
    /// \param int core id to pin the thread affinity onto
    /// \param flag atomic state flag to control execution state
    std::shared_ptr<std::thread> Start(int core_id, std::atomic<uint8_t>& flag);

    /// \brief Iterates over the input queue until a packet with length 0 is
    /// received each decoded itch message is pushed into the output lock free
    /// queue
    /// \param flag atomic state flag to control execution state
    auto Execute(std::atomic<uint8_t>& flag) -> void;

    /// \brief Decodes a single ITCH Message from the raw buffer
    /// \param buffer Pointer to the raw bytes
    /// \return ITCH Message variant
    static ITCH::Message DecodeMessage(const uint8_t* buffer);

   private:
    std::shared_ptr<LockFreeQueue<RawItchPacket>> m_InputQueue;
    std::shared_ptr<LockFreeQueue<ITCH::Message>> m_OutputQueue;

    /// \brief Parsing helpers
    static inline auto SystemEventMessage(const uint8_t* buffer)
        -> ITCH::SystemEventMessage;
    static inline auto StockTradingActionMessage(const uint8_t* buffer)
        -> ITCH::StockTradingActionMessage;
    static inline auto RegSHOMessage(const uint8_t* buffer)
        -> ITCH::RegSHOMessage;
    static inline auto MarketParticipantPositionMessage(const uint8_t* buffer)
        -> ITCH::MarketParticipantPositionMessage;
    static inline auto MWCBDeclineLevelMessage(const uint8_t* buffer)
        -> ITCH::MWCBDeclineLevelMessage;
    static inline auto MWCBStatusMessage(const uint8_t* buffer)
        -> ITCH::MWCBStatusMessage;
    static inline auto StockDirectoryMessage(const uint8_t* buffer)
        -> ITCH::StockDirectoryMessage;
    static inline auto IPOQuotingPeriodUpdateMessage(const uint8_t* buffer)
        -> ITCH::IPOQuotingPeriodUpdateMessage;
    static inline auto LULDAuctionCollarMessage(const uint8_t* buffer)
        -> ITCH::LULDAuctionCollarMessage;
    static inline auto OperationalHaltMessage(const uint8_t* buffer)
        -> ITCH::OperationalHaltMessage;
    static inline auto AddOrderMessage(const uint8_t* buffer)
        -> ITCH::AddOrderMessage;
    static inline auto AddOrderMPIDAttributionMessage(const uint8_t* buffer)
        -> ITCH::AddOrderMPIDAttributionMessage;
    static inline auto OrderExecutedMessage(const uint8_t* buffer)
        -> ITCH::OrderExecutedMessage;
    static inline auto OrderExecutedWithPriceMessage(const uint8_t* buffer)
        -> ITCH::OrderExecutedWithPriceMessage;
    static inline auto OrderCancelMessage(const uint8_t* buffer)
        -> ITCH::OrderCancelMessage;
    static inline auto OrderDeleteMessage(const uint8_t* buffer)
        -> ITCH::OrderDeleteMessage;
    static inline auto OrderReplaceMessage(const uint8_t* buffer)
        -> ITCH::OrderReplaceMessage;
    static inline auto NonCrossTradeMessage(const uint8_t* buffer)
        -> ITCH::NonCrossTradeMessage;
    static inline auto CrossTradeMessage(const uint8_t* buffer)
        -> ITCH::CrossTradeMessage;
    static inline auto BrokenTradeMessage(const uint8_t* buffer)
        -> ITCH::BrokenTradeMessage;
    static inline auto NOIIMessage(const uint8_t* buffer) -> ITCH::NOIIMessage;
    static inline auto DLCRMessage(const uint8_t* buffer) -> ITCH::DLCRMessage;
    static inline auto RetailPriceImprovementIndicatorMessage(
        const uint8_t* buffer) -> ITCH::RetailPriceImprovementIndicatorMessage;
};

#endif  // SOLO_STRATEGY_SRC_ITCH_PARSER_ITCH_PARSER_H_