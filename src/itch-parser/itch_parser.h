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
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

// local includes
#include "itch-parser/messages/itch_messages.h"
#include "lock-free-queue/lock_free_queue.h"
#include "memory-map/memory_map_file.h"

class ITCH_Parser {
   public:
    ITCH_Parser(const std::string& file_path,
                std::shared_ptr<LockFreeQueue<ITCH::Message>>& queue_pointer);

    /// \brief Executes the Execute() function on the newly created thread
    /// \param int core id to pin the thread affinity onto
    std::shared_ptr<std::thread> Start(int core_id);

    /// \brief Iterates over the length of a binary file until it's completed
    /// each decoded itch message is pushed into the lock free queue
    auto Execute();

    /// \brief Decodes a single ITCH Message from the file buffer
    /// \return ITCH Message variant
    ITCH::Message DecodeMessage();

   private:
    MemoryMappedFile m_File;
    std::shared_ptr<LockFreeQueue<ITCH::Message>> m_MessageQueue;

    /// \brief The system event message type is used to signal a market or data
    /// feed handler event
    /// \return ITCH::Message Variant<SystemEventMessage>
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

    /// \brief This message will be generated for unattributed orders accepted
    /// by the Nasdaq system. (Note: If a firm wants to display a MPID for
    /// unattributed orders, Nasdaq recommends that it use the MPID of “NSDQ”.)
    /// \return ITCH::Message Variant<AddOrderMessage>
    inline auto AddOrderMessage() -> ITCH::Message;

    /// \brief This message will be generated for attributed orders and
    /// quotations accepted by the Nasdaq system.
    /// \return ITCH::Message Variant<AddOrderMPIDAttributionMessage>
    inline auto AddOrderMPIDAttributionMessage() -> ITCH::Message;

    /// \brief This message is sent whenever an order on the book is executed in
    /// whole or in part. It is possible to receive several Order Executed
    /// Messages for the same order reference number if that order is executed
    /// in several parts. The multiple Order Executed Messages on the same order
    /// are cumulative.
    /// \return ITCH::Message Variant<OrderExecutedMessage>
    inline auto OrderExecutedMessage() -> ITCH::Message;

    /// \brief This message is sent whenever an order on the book is executed
    /// in whole or in part at a price different from the initial display price.
    /// Since the execution price is different than the display price of the
    /// original Add Order, Nasdaq includes a price field within this execution
    /// message.
    /// \return ITCH::Message Variant<OrderExecutedWithPriceMessage>
    inline auto OrderExecutedWithPriceMessage() -> ITCH::Message;

    /// \brief This message is sent whenever an order on the book is modified as
    /// a result of a partial cancellation.
    /// \return ITCH::Message Variant<OrderCancelMessage>
    inline auto OrderCancelMessage() -> ITCH::Message;

    /// \brief This message is sent whenever an order on the book is being
    /// cancelled. All remaining shares are no longer accessible so the order
    /// must be removed from the book.
    /// \return ITCH::Message Variant<OrderDeleteMessage>
    inline auto OrderDeleteMessage() -> ITCH::Message;

    /// \brief This message is sent whenever an order on the book has been
    /// cancel replaced. All remaining shares from the original order are no
    /// longer accessible, and must be removed. The new order details are
    /// provided for the replacement, along with a new order reference number
    /// which will be used henceforth. Since the side, stock symbol and
    /// attribution (if any) cannot be changed by an Order Replace event, these
    /// fields are not included in the message. Firms should retain the
    /// side,stock symbol and MPID from the original Add Order message.
    /// \return ITCH::Message Variant<OrderReplaceMessage>
    inline auto OrderReplaceMessage() -> ITCH::Message;

    /// \brief The Trade Message is designed to provide execution details for
    /// normal match events involving nondisplayable order types.
    /// \return ITCH::Message Variant<NonCrossTradeMessage>
    inline auto NonCrossTradeMessage() -> ITCH::Message;

    /// \brief The Trade Message is designed to provide execution details for
    /// normal match events involving nondisplayable order types.
    /// \return ITCH::Message Variant<CrossTradeMessage>
    inline auto CrossTradeMessage() -> ITCH::Message;

    /// \brief The Broken Trade Message is sent whenever an execution on Nasdaq
    /// is broken. An execution may be broken if it is found to be “clearly
    /// erroneous” pursuant to Nasdaq’s Clearly Erroneous Policy. A trade break
    /// is final; once a trade is broken, it cannot be reinstated.
    /// \return ITCH::Message Variant<BrokenTradeMessage>
    inline auto BrokenTradeMessage() -> ITCH::Message;

    /// \brief Nasdaq begins disseminating Net Order Imbalance Indicators (NOII)
    /// at 9:25 a.m. for the Opening Cross and 3:50 p.m. for the Closing Cross.
    /// • Between 9 : 25 and 9 : 28 a.m.and 3 : 50 and 3 : 55 p.m.,
    /// Nasdaq disseminates the NOII information every 10 seconds.
    /// • Between 9 : 28 and 9 : 30 a.m.and 3 : 55 and 4 : 00 p.m.,
    /// Nasdaq disseminates the NOII information every second.
    /// • For Nasdaq Halt, IPO and Pauses,
    /// NOII messages will be disseminated at 1 second intervals
    /// starting 1 second after quoting period starts /
    /// trading action is released.
    /// \return ITCH::Message Variant<NOIIMessage>
    inline auto NOIIMessage() -> ITCH::Message;

    /// \brief The following message is disseminated only for Direct Listing
    /// with Capital Raise (DLCR) securities. Nasdaq begins disseminating
    /// messages once per second as soon as the DLCR volatility test has
    /// successfully passed.
    /// \return ITCH::Message Variant<DLCRMessage>
    inline auto DLCRMessage() -> ITCH::Message;

    /// \brief Identifies a retail interest indication of the Bid, Ask or both
    /// the Bid and Ask for Nasdaq listed securities.
    /// \return ITCH::Message Variant<RetailPriceImprovementIndicatorMessage>
    inline auto RetailPriceImprovementIndicatorMessage() -> ITCH::Message;
};

#endif  // SOLO_STRATEGY_SRC_ITCH_PARSER_ITCH_PARSER_H_