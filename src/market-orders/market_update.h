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

/// @file market_update.h
/// @brief Messaging structures for publishing and consuming market data
/// updates.

#ifndef SOLO_STRATEGY_SRC_MARKET_ORDERS_MARKET_UPDATE_H_
#define SOLO_STRATEGY_SRC_MARKET_ORDERS_MARKET_UPDATE_H_

#include <sstream>

#include "lock-free-queue/lock_free_queue.h"
#include "utilities/types.h"

/// @brief Represents the type of action in a market update message.
enum class MarketUpdateType : uint8_t {
    INVALID = 0,
    CLEAR = 1,
    ADD = 2,
    MODIFY = 3,
    CANCEL = 4,   ///< Partial quantity reduction (ITCH OrderCancelMessage).
    DELETED = 5,  ///< Full order removal (ITCH OrderDeleteMessage).
    TRADE = 6,    ///< Order execution (ITCH OrderExecuted[WithPrice]Message).
    SNAPSHOT_START = 7,
    SNAPSHOT_END = 8
};

/// @brief Converts a MarketUpdateType enum to its string representation.
/// @param type The MarketUpdateType to convert.
/// @return String representation of the type.
inline std::string MarketUpdateTypeToString(MarketUpdateType type) {
    switch (type) {
        case MarketUpdateType::CLEAR:
            return "CLEAR";
        case MarketUpdateType::ADD:
            return "ADD";
        case MarketUpdateType::MODIFY:
            return "MODIFY";
        case MarketUpdateType::CANCEL:
            return "CANCEL";
        case MarketUpdateType::DELETED:
            return "DELETE";
        case MarketUpdateType::TRADE:
            return "TRADE";
        case MarketUpdateType::SNAPSHOT_START:
            return "SNAPSHOT_START";
        case MarketUpdateType::SNAPSHOT_END:
            return "SNAPSHOT_END";
        case MarketUpdateType::INVALID:
            return "INVALID";
    }
    return "UNKNOWN";
}
/// \brief These structures go over the wire / network, so the binary structures
/// are packed to remove system dependent extra padding.
#pragma pack(push, 1)

/// @brief Market update structure used internally.
/// Represents a normalized, protocol-agnostic view of a single order-book
/// event derived from an ITCH 5.0 message.  The BooksManager layer is
/// responsible for translating raw ITCH structures into this form before
/// dispatching to the relevant OrderBook.
struct MarketUpdate {
    /// @brief The kind of order-book event this update represents.
    MarketUpdateType type = MarketUpdateType::INVALID;

    /// @brief Exchange order reference number (ITCH order_reference_number).
    /// For MODIFY events this holds the *new* order reference number.
    OrderId order_id = OrderId_INVALID;

    /// @brief Internal ticker index identifying the instrument.
    TickerId ticker_id = TickerId_INVALID;

    /// @brief Order side: BUY ('B') or SELL ('S').
    /// Populated for ADD and MODIFY only; INVALID for CANCEL/DELETE/TRADE.
    Side side = Side::INVALID;

    /// @brief Limit price in 4-decimal fixed-point units (matching ITCH
    /// encoding). For TRADE events this holds the execution price.
    Price price = Price_INVALID;

    /// @brief Share quantity.
    Qty qty = Qty_INVALID;

    /// @brief Time-priority within a price level; assigned by the BooksManager.
    Priority priority = Priority_INVALID;

    /// @brief Converts the MarketUpdate to a string representation.
    /// @return String representation of the update.
    auto ToString() const {
        std::stringstream stream;
        stream << "MarketUpdate"
               << " ["
               << " type:" << MarketUpdateTypeToString(type)
               << " ticker:" << TickerIdToString(ticker_id)
               << " oid:" << OrderIdToString(order_id)
               << " side:" << SideToString(side) << " qty:" << QtyToString(qty)
               << " price:" << PriceToString(price)
               << " priority:" << PriorityToString(priority) << "]";
        return stream.str();
    }
};

/// @brief Market update structure published over the network.
/// Includes a sequence number for tracking and ordering.
struct MDPMarketUpdate {
    size_t seq_num_ = 0;
    MarketUpdate market_update_;

    /// @brief Converts the MDPMarketUpdate to a string representation.
    /// @return String representation of the update.
    auto ToString() const {
        std::stringstream stream;
        stream << "MDPMarketUpdate"
               << " ["
               << " seq:" << seq_num_ << " " << market_update_.ToString()
               << "]";
        return stream.str();
    }
};

/// \brief Undo the packed binary structure directive moving forward.
#pragma pack(pop)

/// \typedef MarketUpdateLFQueue
/// \brief Lock free queue of matching engine market update messages.
typedef LockFreeQueue<MarketUpdate> MarketUpdateLFQueue;

/// \typedef MDPMarketUpdateLFQueue
/// \brief Lock free queue of market data publisher market update messages.
typedef LockFreeQueue<MDPMarketUpdate> MDPMarketUpdateLFQueue;

#endif  // SOLO_STRATEGY_SRC_MARKET_ORDERS_MARKET_UPDATE_H_
